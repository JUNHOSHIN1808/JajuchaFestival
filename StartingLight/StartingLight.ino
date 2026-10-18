/*
  StartLight -- Classic Arduino Nano (ATmega328P, 5 V / 16 MHz)
  Four MAX7219 8 x 8 modules form one 8 x 32 board, mounted vertically.

  Wiring:
    MAX7219 DIN -> D11, CLK -> D13, CS/LOAD -> D10
    MAX7219 VCC -> regulated 5 V distribution rail; GND -> common GND.
    Momentary pushbutton -> D2 and GND (INPUT_PULLUP).
    Buzzer transistor base resistor -> D5; transistor emitter -> GND.
    Power the buzzer from its rated supply, NOT directly from an Arduino pin.
    See the construction guide for transistor wiring and flyback diode.

  Required sequence:
    Press -> bottom block ON immediately, then another block every 1500 ms.
    Blocks accumulate: 1, 2, 3, 4 at t = 0, 1.5, 3.0, 4.5 seconds.
    Once all four are ON, wait a random 3000...6000 ms, then all OFF = START.
    Sound once for 100 ms when each block is added (four short sounds),
    then once for 800 ms when all blocks turn OFF (one long sound).
    Ignore presses during the sequence and during a 3 s post-start lockout.
    A fresh press after lockout starts the next sequence.

  This uses the bundled SPI library; no MAX7219 library is required.
  Buzzer is enabled by default as requested; the manual's light timing remains.
  Actual electrical safety, direction, visibility and timing need bench tests.
*/

#include <Arduino.h>
#include <SPI.h>

const uint8_t PIN_CS = 10;
const uint8_t PIN_BUTTON = 2;
const uint8_t PIN_MANUAL_BUTTON = 7;  // 추가 버튼: D7과 GND 사이에 연결
const uint8_t PIN_BUZZER = 5;
const uint8_t MODULES = 4;

// Device 0 is nearest DIN from the Nano; device 3 is at the far chain end.
// If illumination starts at the top, change this from 0 to 3.
const uint8_t BOTTOM_DEVICE = 0;
static_assert(BOTTOM_DEVICE == 0 || BOTTOM_DEVICE == 3,
              "BOTTOM_DEVICE must be 0 or 3.");

const uint8_t INTENSITY = 4;  // 0...15; start dim and verify current/visibility.
static_assert(INTENSITY <= 15, "INTENSITY must be 0...15.");
const bool ENABLE_BUZZER = true;
// Set true only for an active buzzer that sounds from DC.
// false = passive magnetic/piezo buzzer: 2000 Hz tone via transistor.
const bool ACTIVE_BUZZER = false;

const uint32_t STEP_MS = 1500UL;
const uint32_t HOLD_MIN_MS = 3000UL;
const uint32_t HOLD_MAX_MS = 6000UL;
const uint32_t LOCKOUT_MS = 3000UL;
const uint32_t DEBOUNCE_MS = 30UL;
const uint32_t SHORT_BEEP_MS = 100UL;
const uint32_t START_BEEP_MS = 800UL;

enum RunState : uint8_t { IDLE, ADDING_BLOCKS, ALL_ON_HOLD, LOCKOUT };
RunState runState = IDLE;
uint32_t sequenceStartedMs = 0;
uint32_t holdStartedMs = 0;
uint32_t holdDurationMs = 0;
uint32_t lightsOffMs = 0;
uint8_t shownBlocks = 0;
uint8_t manualBlocks = 0;  // 수동 단계: 0=모두 꺼짐, 1~4=아래부터 켜진 칸 수

uint8_t buttonRaw = HIGH;
uint8_t buttonStable = HIGH;
uint32_t buttonChangedMs = 0;
bool buttonReleased = true;
// D7은 D2와 별도로 누름/해제 상태를 기억합니다. 접점 떨림은 DEBOUNCE_MS로 거릅니다.
uint8_t manualButtonRaw = HIGH;
uint8_t manualButtonStable = HIGH;
uint32_t manualButtonChangedMs = 0;
bool manualButtonReleased = true;
bool buzzerOn = false;
uint32_t buzzerStartedMs = 0;
uint32_t buzzerDurationMs = 0;

// One LOAD rising edge latches the same register in all four devices.
// Byte pairs are sent farthest-first: device 3, 2, 1, 0.
void writeRegister(uint8_t address, const uint8_t values[MODULES]) {
  SPI.beginTransaction(SPISettings(1000000UL, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_CS, LOW);
  for (int8_t device = MODULES - 1; device >= 0; --device) {
    SPI.transfer(address);
    SPI.transfer(values[device]);
  }
  digitalWrite(PIN_CS, HIGH);
  SPI.endTransaction();
}

void writeAll(uint8_t address, uint8_t value) {
  uint8_t values[MODULES];
  for (uint8_t device = 0; device < MODULES; ++device) values[device] = value;
  writeRegister(address, values);
}

void lightsOff() {
  // MAX7219 shutdown blanks the four displays at the SAME LOAD edge.
  // Also clear row RAM so emulators that do not render shutdown correctly
  // visibly go dark, and the next sequence begins with an empty display.
  writeAll(0x0C, 0x00);
  for (uint8_t row = 1; row <= 8; ++row) writeAll(row, 0x00);
}

void showBlocks(uint8_t count) {
  uint8_t values[MODULES] = { 0, 0, 0, 0 };
  for (uint8_t rank = 0; rank < count; ++rank) {
    uint8_t device = (BOTTOM_DEVICE == 0) ? rank : MODULES - 1 - rank;
    values[device] = 0xFF;
  }
  for (uint8_t row = 1; row <= 8; ++row) writeRegister(row, values);
  writeAll(0x0C, 0x01);  // normal operation, after the rows are prepared
}

void pulseBuzzer(uint32_t now, uint32_t durationMs) {
  if (!ENABLE_BUZZER) return;
  if (ACTIVE_BUZZER) digitalWrite(PIN_BUZZER, HIGH);
  else tone(PIN_BUZZER, 2000);
  buzzerOn = true;
  buzzerStartedMs = now;
  buzzerDurationMs = durationMs;
}

void updateBuzzer(uint32_t now) {
  if (buzzerOn && (uint32_t)(now - buzzerStartedMs) >= buzzerDurationMs) {
    if (!ACTIVE_BUZZER) noTone(PIN_BUZZER);
    digitalWrite(PIN_BUZZER, LOW);
    buzzerOn = false;
  }
}

bool freshButtonPress(uint32_t now) {
  uint8_t reading = digitalRead(PIN_BUTTON);
  if (reading != buttonRaw) {
    buttonRaw = reading;
    buttonChangedMs = now;
  }
  if (reading != buttonStable &&
      (uint32_t)(now - buttonChangedMs) >= DEBOUNCE_MS) {
    buttonStable = reading;
    if (buttonStable == HIGH) buttonReleased = true;
    else if (buttonReleased) {
      buttonReleased = false;
      return true;
    }
  }
  return false;
}

// D7을 길게 누르는 동안 반복 점등되지 않게 '새로 누른 순간'에만 true를 반환합니다.
// 자동 진행 중에도 호출해서, 무시한 누름이 나중에 뒤늦게 실행되지 않도록 합니다.
bool freshManualButtonPress(uint32_t now) {
  uint8_t reading = digitalRead(PIN_MANUAL_BUTTON);
  if (reading != manualButtonRaw) {
    manualButtonRaw = reading;
    manualButtonChangedMs = now;
  }
  if (reading != manualButtonStable &&
      (uint32_t)(now - manualButtonChangedMs) >= DEBOUNCE_MS) {
    manualButtonStable = reading;
    if (manualButtonStable == HIGH) manualButtonReleased = true;
    else if (manualButtonReleased) {
      manualButtonReleased = false;
      return true;
    }
  }
  return false;
}

// 버튼 5회로 한 바퀴: 1칸 → 2칸 → 3칸 → 4칸 → 꺼짐. 소리나 자동 타이머는 시작하지 않습니다.
void advanceManualLight() {
  ++manualBlocks;
  if (manualBlocks > MODULES) manualBlocks = 0;
  shownBlocks = manualBlocks;
  if (manualBlocks == 0) lightsOff();
  else showBlocks(manualBlocks);
}

void beginSequence(uint32_t now) {
  manualBlocks = 0;  // 자동 동작이 끝나면 다음 수동 점등은 항상 1칸부터 시작
  // Floating A0 adds variability; operator press timing adds another source.
  // This is suitable for a start-light delay, not a cryptographic generator.
  randomSeed((micros() ^ ((uint32_t)analogRead(A0) << 20)) | 1UL);
  holdDurationMs = (uint32_t)random(HOLD_MIN_MS, HOLD_MAX_MS + 1UL);
  sequenceStartedMs = now;
  shownBlocks = 1;
  showBlocks(shownBlocks);
  pulseBuzzer(now, SHORT_BEEP_MS);
  runState = ADDING_BLOCKS;
}

void setup() {
  digitalWrite(PIN_CS, HIGH);
  pinMode(PIN_CS, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_BUTTON, INPUT_PULLUP);
  buttonRaw = buttonStable = digitalRead(PIN_BUTTON);
  buttonReleased = (buttonStable == HIGH);  // held at boot does not start
  buttonChangedMs = millis();

  pinMode(PIN_MANUAL_BUTTON, INPUT_PULLUP);
  manualButtonRaw = manualButtonStable = digitalRead(PIN_MANUAL_BUTTON);
  manualButtonReleased = (manualButtonStable == HIGH);
  manualButtonChangedMs = millis();

  SPI.begin();
  lightsOff();
  writeAll(0x0F, 0x00);  // display-test mode OFF (do not flash all LEDs)
  writeAll(0x09, 0x00);  // no BCD decode for a dot matrix
  writeAll(0x0B, 0x07);  // scan all 8 rows
  writeAll(0x0A, INTENSITY);
  for (uint8_t row = 1; row <= 8; ++row) writeAll(row, 0x00);
  // Remain in shutdown until the first button press.
}

void loop() {
  uint32_t now = millis();
  bool pressed = freshButtonPress(now);  // always consume presses, even busy
  bool manualPressed = freshManualButtonPress(now);  // 자동 진행/잠금 중에도 입력 상태 갱신
  updateBuzzer(now);

  switch (runState) {
    case IDLE:
      if (pressed) beginSequence(now);
      else if (manualPressed) advanceManualLight();  // D2와 동시에 누르면 D2 자동 시작 우선
      break;

    case ADDING_BLOCKS: {
      uint32_t elapsed = (uint32_t)(now - sequenceStartedMs);
      uint8_t wanted = (uint8_t)(elapsed / STEP_MS + 1UL);
      if (wanted > MODULES) wanted = MODULES;
      if (wanted != shownBlocks) {
        shownBlocks = wanted;
        showBlocks(shownBlocks);
        pulseBuzzer(now, SHORT_BEEP_MS);
      }
      if (shownBlocks == MODULES) {
        holdStartedMs = sequenceStartedMs + STEP_MS * (MODULES - 1UL);
        runState = ALL_ON_HOLD;
      }
      break;
    }

    case ALL_ON_HOLD:
      if ((uint32_t)(now - holdStartedMs) >= holdDurationMs) {
        lightsOff();
        shownBlocks = 0;
        lightsOffMs = millis();
        pulseBuzzer(lightsOffMs, START_BEEP_MS);
        runState = LOCKOUT;
      }
      break;

    case LOCKOUT:
      if ((uint32_t)(now - lightsOffMs) >= LOCKOUT_MS) runState = IDLE;
      break;
  }
}
