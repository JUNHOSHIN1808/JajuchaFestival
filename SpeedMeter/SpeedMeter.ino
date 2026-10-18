/*
  [센서 배선]
  실제 센서의 5볼트 정격, NPN 오픈 컬렉터 출력, 선색 표기를 먼저 확인하세요.
  해당 조건에서 갈색은 안정된 5볼트, 파랑은 공통 접지, 검정은 출력 노드입니다.
  각 출력 노드 → 4.7킬로옴 → 우노와 같은 5볼트 전원으로 외부 풀업을 연결합니다.
  같은 출력 노드 → 1킬로옴 직렬 저항 → 앞 센서 D2 / 뒤 센서 D3로 연결합니다.
  1킬로옴 직렬 저항 대신 보유한 330옴 세 개를 직렬 연결한 990옴을 사용할 수 있습니다.
  외부 4.7킬로옴이 없으면, 실제 5볼트 NPN 상시 열림(NO) 오픈 컬렉터로 확인된 센서에 한해
  내부 INPUT_PULLUP으로 짧은 배선의 초기 감지 시험을 할 수 있습니다.
  이 시험 구성은 현장 잡음 내성이나 긴 배선의 정상 동작을 보증하지 않습니다.
  실물 기본 설계의 외부 4.7킬로옴 풀업은 유지하며 현장 설치 전 입력 안정성을 확인하세요.
  내부 풀업도 사용합니다. 감지하면 LOW, 센서를 비우면 HIGH가 됩니다.
  차량 진행 방향에 앞 센서 A, 그다음 뒤 센서 B 순서로 설치합니다.

  [버튼·매트릭스·부저 배선]
  MAX7219 8×8 모듈 네 개: DIN D11 / CLK D13 / CS 또는 LOAD D10.
  매트릭스에는 별도 5볼트 분배선으로 전원을 공급하고 우노와 접지를 공유합니다.
  D5는 베이스 저항을 거쳐 NPN 트랜지스터 부저 구동부로 연결합니다.
  실물 자기식 부저의 전류를 D5에서 직접 공급하지 않습니다. 부저 정격, 트랜지스터
  단자 순서, 베이스 저항, 역기전력 보호 다이오드는 실물 배선 안내를 따르세요.
  기본은 수동 부저용 2000헤르츠 신호입니다. 확인된 능동 부저만 설정을 바꿉니다.

  [배터리 전원과 USB 모드]
  1셀 3.7볼트 2600밀리암페어시 배터리 → 보호 회로 → 퓨즈·스위치
  → MT3608의 조정된 5볼트 출력 → 전원 분배 순서입니다.
  배터리를 우노 5V에 직접 연결하지 않습니다. 부하 연결 전과 연결 후 전압을 확인합니다.
  USB 연결 모드에서는 실물 안내의 J1을 제거하여 외부 5볼트 역급전을 막습니다.
  J1은 실물 전원 분리점이며 이 코드로 대신 제어할 수 없습니다.

  [동작과 표시 우선순위]
  처음에는 정지 상태입니다. D4와 GND 사이 버튼을 눌러 시작/정지를 전환합니다.
  두 센서가 100ms 동안 비워지면 준비됩니다. 앞→뒤 시간으로 측정하며 역방향,
  동시 감지, 5초 초과는 취소합니다. PASS는 계속 켜고 OVER는 점멸합니다.
  D6와 GND 사이 직전 속도 확인 버튼은 마지막 유효 측정 속도를 소수 한 자리로 반올림해
  3초 표시합니다(예: 29.7, 단위 cm/s). 아직 기록이 없으면 ----를 표시합니다.
  유효 기록은 정지/취소/제한 변경 뒤에도 유지되며 전원을 끄거나 재부팅하면 사라집니다.
  조회는 센서 시각이나 부저를 건드리지 않습니다. 숫자 표시 중에도 측정을 계속합니다.
  새 유효 결과가 나오면 숫자를 즉시 대체합니다. 숫자 표시가 끝나면 아직 유효한
  PASS/OVER를 원래 종료 시각과 OVER 점멸 위상에 맞춰 복원합니다.
  숫자 표시 중 기존 결과 시간이 끝나면 정상적으로 비움 대기로 넘어갑니다.
  표시 우선순위: D4 전환/측정 취소의 소등 → 새 유효 결과 → 숫자 조회/설정 → 기존 결과.
  준비/앞 센서 대기는 숫자를 지우지 않습니다. 폭이 32열을 넘는 숫자는 3초 안에
  처음부터 끝까지 가로로 이동하여 표시하므로 자릿수를 버리거나 값을 바꾸지 않습니다.
  모든 버튼은 내부 INPUT_PULLUP이며, 부팅 때 눌린 버튼은 떼었다 다시 눌러야 작동합니다.
  배포용에는 조회 버튼 한 개만 추가하며 제한 설정 버튼을 연결하지 않습니다.
  고정 제한은 코드 수정/업로드로만 바뀝니다. D4 전환 시 진행 중인 조회 누름도 무효로 합니다.

  [시리얼·시간 처리]
  시리얼 모니터는 115200 baud, UTF-8입니다. 수신 명령은 없습니다.
  인터럽트에서는 시각과 플래그만 기록합니다. delay()나 동적 String을 사용하지 않습니다.
  부호 없는 32비트 뺄셈으로 micros()/millis() 카운터 넘침을 처리합니다.
*/

#include <Arduino.h>
#include <SPI.h>
#include <util/atomic.h>
#include <avr/pgmspace.h>

// 핀 설정: 우노의 외부 인터럽트 입력 D2/D3로 감지 시각을 기록합니다.
// 앞 센서: D2 외부 인터럽트 사용. 다른 일반 핀으로 숫자만 바꾸지 마세요.
const uint8_t PIN_SENSOR_A = 2;
// 뒤 센서: D3 외부 인터럽트 사용. 아래 EIFR의 INTF0/INTF1 처리도 이 핀 구성을 전제로 합니다.
const uint8_t PIN_SENSOR_B = 3;
const uint8_t PIN_RUN_BUTTON = 4;
const uint8_t PIN_BUZZER = 5;
// 직전 속도 확인: D6와 접지 사이에 순간 누름 버튼을 연결합니다.
const uint8_t PIN_RECALL_BUTTON = 6;

// CS 변경 시 배선도 함께 변경하고, 하드웨어 SPI의 D10 출력 유지 및 핀 충돌을 확인하세요.
const uint8_t PIN_MATRIX_CS = 10;
// 네 모듈에 한 글자씩 표시합니다. 개수만 바꾸지 말고 문구 배치와 전원도 함께 재설계하세요.
const uint8_t MATRIX_MODULES = 4;
// 기본 FC16 방향은 DIN 쪽이 오른쪽입니다. SPI 장치 0은 DIN에 가장 가까운 모듈입니다.
// 글자 모양은 바른데 SSAP처럼 순서만 뒤집히면 이 값을 false로 변경하세요.
const bool MATRIX_DIN_ON_RIGHT = true;
// 각 글자의 시계 방향 90도 회전 횟수: 0=그대로, 1=90도, 2=180도, 3=270도.
// Wokwi layout="fc16"의 기본 방향과 일반 FC16에는 0을 사용하고 실물 장착 결과로 조정하세요.
const uint8_t MATRIX_ROTATION = 0;
// 회전 적용 후 좌우 거울상 보정. 글자 자체가 좌우 뒤집힌 경우에만 true로 바꿉니다.
const bool MATRIX_MIRROR_X = false;
// 밝기 0~15. 0은 최저 밝기이며 완전 소등은 matrixOff()가 담당합니다. 처음에는 1로 확인하세요.
const uint8_t MATRIX_INTENSITY = 1;  // 0~15 중 낮은 초기 밝기 1: 배터리 전류 부담을 줄입니다.

// 사용자 설정: 센서 몸체가 아닌 실제 감지 지점 사이의 간격입니다.
const uint32_t DISTANCE_MM = 300UL;  // 300mm = 30cm. 실제 설치 간격과 반드시 맞추세요.
const uint32_t DEFAULT_SPEED_LIMIT_CM_S = 21UL;  // 기본 제한 21cm/s, 같은 속도는 정상입니다.
const uint32_t MIN_SPEED_LIMIT_CM_S = 10UL;     // 제한 설정의 하한 10cm/s.
const uint32_t MAX_SPEED_LIMIT_CM_S = 500UL;    // 제한 설정의 상한 500cm/s.
// 배포용은 고정 제한입니다. 변경하려면 위 기본값을 고친 뒤 다시 업로드하세요.
const uint32_t speedLimitCmS = DEFAULT_SPEED_LIMIT_CM_S;
// 수동 부저는 false, 직류만으로 발진하는 능동 부저로 확인된 경우만 true입니다.
const bool ACTIVE_BUZZER = false;
// _MS는 밀리초, _US는 마이크로초입니다. 아래 시간은 모두 기다림 없이 처리합니다.
const uint32_t CLEAR_MS = 100UL;             // 두 센서가 비워진 뒤 재준비하는 시간.
const uint32_t BUTTON_DEBOUNCE_MS = 30UL;    // 버튼 접점 튐 제거 시간.
const uint32_t MIN_INTERVAL_US = 1000UL;     // 1ms 미만의 동시 감지/잡음 취소.
const uint32_t TIMEOUT_US = 5000000UL;       // 5초 초과 시 취소; 수정 시 안내 문구도 고치세요.
// 30cm / 5초 = 6cm/s까지 시간 범위에 들며, 제한 설정 하한 10cm/s와는 별도입니다.
const uint32_t PASS_BEEP_MS = 120UL;         // 정상 통과의 짧은 소리.
const uint32_t VIOLATION_BEEP_MS = 1000UL;   // 과속의 긴 소리.
const uint32_t RESULT_MS = 3000UL;           // PASS/OVER 결과의 전체 표시 시간.
const uint32_t BLINK_HALF_MS = 250UL;        // OVER 켜짐/꺼짐 각각의 시간, 0 금지.
const uint32_t RECENT_DISPLAY_MS = 3000UL;   // D6 조회 숫자의 표시 시간.

static_assert(DISTANCE_MM > 0 && DISTANCE_MM <= 100000UL, "Invalid distance.");
static_assert(MIN_SPEED_LIMIT_CM_S > 0 &&
              MIN_SPEED_LIMIT_CM_S <= DEFAULT_SPEED_LIMIT_CM_S &&
              DEFAULT_SPEED_LIMIT_CM_S <= MAX_SPEED_LIMIT_CM_S, "Default limit outside range.");
static_assert(MATRIX_MODULES == 4 && MATRIX_ROTATION < 4, "Four modules; rotation 0..3 required.");
static_assert(MATRIX_INTENSITY <= 15, "MAX7219 intensity must be 0..15.");
static_assert(MIN_INTERVAL_US > 0 && MIN_INTERVAL_US <= TIMEOUT_US && TIMEOUT_US < 0x80000000UL,
              "Use a positive, wrap-safe sensor interval.");
static_assert(((uint64_t)DISTANCE_MM * 1000000ULL + MIN_INTERVAL_US / 2UL) / MIN_INTERVAL_US <= 0xFFFFFFFFULL,
              "Rounded recall speed must fit uint32_t.");
static_assert(BLINK_HALF_MS > 0 && RESULT_MS > 0 && RESULT_MS < 0x80000000UL &&
              RECENT_DISPLAY_MS >= 2 && RECENT_DISPLAY_MS < 0x80000000UL,
              "Use positive, wrap-safe display times.");

// 상태 순서: 정지 → 비움 대기 → 준비 → 뒤 센서 대기 → 결과 → 비움 대기.
// 취소는 결과 없이 비움 대기로 돌아가고, 버튼은 모든 상태에서 정지할 수 있습니다.
enum MeasureState : uint8_t { STOPPED, WAIT_CLEAR, ARMED, WAIT_B, RESULT };
MeasureState measureState = STOPPED;
bool running = false;
// ISR과 주 반복문이 공유하므로 volatile입니다.
// 우노는 8비트이므로 여러 바이트의 일관된 읽기에는 원자 구간이 필요합니다.
volatile bool captureEnabled = false;
volatile bool seenA = false, seenB = false;
volatile uint32_t timestampA = 0, timestampB = 0;
volatile uint8_t firstSensor = 0;  // 1: 앞 먼저 / 2: 뒤 먼저 / 3: 최초 감지부터 겹침
// 측정하지 않을 때도 감지 횟수를 증가시켜 짧은 재감지가 비움 대기를 초기화하게 합니다.
volatile uint16_t sensorActivity = 0;
// 같은 순간에 복사한 감지 정보 묶음입니다. 서로 다른 시점의 시각이 섞이지 않게 합니다.
struct Capture {
  bool a, b;
  uint32_t at, bt;
  uint8_t first;
  uint16_t activity;
};

// 주 반복문 전용 상태: 비움·결과·부저·버튼의 시작 시각을 각각 저장합니다.
bool clearTiming = false;
uint32_t clearStartedMs = 0;
uint16_t clearActivity = 0;
uint32_t resultStartedMs = 0;
bool resultViolation = false;
bool displayedOn = false;
bool buzzerOn = false;
uint32_t buzzerStartedMs = 0, buzzerDurationMs = 0;
uint8_t buttonRaw = HIGH, buttonStable = HIGH;
uint32_t buttonChangedMs = 0;
bool buttonReleased = true;
// 기록은 측정 상태/결과 화면과 독립적입니다. 취소나 정지로 덮어쓰지 않습니다.
bool hasLastSpeed = false;
uint32_t lastSpeedTenths = 0;  // 조회 전용 0.1cm/s 단위; 원시 판정에는 사용하지 않습니다.

// 버튼은 원시 입력과 안정 입력을 따로 저장합니다. ready=false이면 먼저 떼어야 합니다.
struct ActionButton {
  uint8_t raw = HIGH, stable = HIGH;
  uint32_t changedMs = 0;
  bool ready = true;
};
ActionButton recallButton;

bool auxiliaryBlocked = false;  // D4 전환 또는 겹침 뒤에는 모든 보조 버튼을 떼어야 합니다.

enum OverlayKind : uint8_t { OVERLAY_NONE, OVERLAY_RECALL };
OverlayKind overlayKind = OVERLAY_NONE;
char overlayText[14] = {};  // 32비트 정수 10자리 + 소수점 + 한 자리 + 종료 문자까지 수용.
uint8_t overlayWidth = 0, overlayLastOffset = 255;
uint32_t overlayStartedMs = 0, overlayDurationMs = 0;
bool displayDirty = false;

// Arduino의 자동 선언 생성에 의존하지 않도록 사용자 정의 형식의 함수는 직접 선언합니다.
Capture readCapture();
void finishMeasurement(const Capture &c);
void initializeActionButton(ActionButton &button, uint8_t pin, uint32_t now);
uint8_t updateActionButton(ActionButton &button, uint8_t reading, uint32_t now);
void beginOverlay(OverlayKind kind, uint32_t now, uint32_t duration);

// 네 MAX7219에 같은 레지스터 값을 SPI로 보냅니다. CS를 올리는 순간 네 모듈이 함께 반영합니다.
void writeMatrixRegister(uint8_t address, uint8_t value) {
  SPI.beginTransaction(SPISettings(1000000UL, MSBFIRST, SPI_MODE0));
  digitalWrite(PIN_MATRIX_CS, LOW);
  for (uint8_t i = 0; i < MATRIX_MODULES; ++i) {
    SPI.transfer(address);
    SPI.transfer(value);
  }
  digitalWrite(PIN_MATRIX_CS, HIGH);
  SPI.endTransaction();
}

// 표시를 절전 소등한 뒤 줄 데이터도 지워 다음 표시를 빈 상태에서 시작합니다.
void matrixOff() {
  writeMatrixRegister(0x0C, 0);  // 한 번의 래치 신호로 네 모듈을 함께 소등
  for (uint8_t row = 1; row <= 8; ++row) writeMatrixRegister(row, 0);
  displayedOn = false;
}

// 논리 프레임은 왼쪽부터 네 모듈, 위부터 여덟 줄이며 비트 7이 각 모듈의 왼쪽입니다.
// 문자/숫자가 같은 전송기를 사용하여 모듈 순서와 8×8 회전/미러 설정을 공유합니다.
void writeMatrixFrame(const uint8_t frame[MATRIX_MODULES][8]) {
  uint8_t pixels[MATRIX_MODULES][8] = {};
  for (uint8_t letter = 0; letter < MATRIX_MODULES; ++letter) {
    uint8_t device = MATRIX_DIN_ON_RIGHT ? MATRIX_MODULES - 1 - letter : letter;
    for (uint8_t y = 0; y < 8; ++y) {
      uint8_t row = frame[letter][y];
      for (uint8_t x = 0; x < 8; ++x) {
        if (!(row & (1U << (7 - x)))) continue;
        uint8_t targetX = x, targetY = y;
        for (uint8_t turn = 0; turn < MATRIX_ROTATION; ++turn) {
          uint8_t oldX = targetX;
          targetX = 7 - targetY;
          targetY = oldX;
        }
        if (MATRIX_MIRROR_X) targetX = 7 - targetX;
        pixels[device][targetY] |= (1U << (7 - targetX));
      }
    }
  }
  for (uint8_t row = 0; row < 8; ++row) {
    SPI.beginTransaction(SPISettings(1000000UL, MSBFIRST, SPI_MODE0));
    digitalWrite(PIN_MATRIX_CS, LOW);
    for (int8_t device = MATRIX_MODULES - 1; device >= 0; --device) {
      SPI.transfer(row + 1);
      SPI.transfer(pixels[device][row]);
    }
    digitalWrite(PIN_MATRIX_CS, HIGH);
    SPI.endTransaction();
  }
  writeMatrixRegister(0x0C, 1);
  displayedOn = true;
}

// 정상은 PASS, 과속은 OVER를 한 모듈에 한 글자씩 표시합니다.
void showResultText(bool violation) {
  // 각 글자는 위에서 아래로 여덟 줄, 각 줄의 비트 7은 왼쪽 점입니다.
  // 예: 0x44=01000100은 왼쪽에서 두 번째와 여섯 번째 점을 켭니다.
  // 0x00은 빈 줄, 0x7C=01111100은 가운데 다섯 점입니다. 글자당 8개 값을 유지하세요.
  // 배열 첫 번째 차원은 왼쪽부터 네 글자입니다. 문구 변경 시 네 글자 패턴을 모두 바꿉니다.
  // PROGMEM은 글꼴을 플래시에 저장하여 작은 우노 SRAM 사용을 줄입니다.
  static const uint8_t passGlyphs[4][8] PROGMEM = {
    {0x78,0x44,0x44,0x78,0x40,0x40,0x40,0x00}, // P
    {0x38,0x44,0x44,0x7C,0x44,0x44,0x44,0x00}, // A
    {0x3C,0x40,0x40,0x38,0x04,0x04,0x78,0x00}, // S
    {0x3C,0x40,0x40,0x38,0x04,0x04,0x78,0x00}  // S
  };
  static const uint8_t overGlyphs[4][8] PROGMEM = {
    {0x38,0x44,0x44,0x44,0x44,0x44,0x38,0x00}, // O
    {0x44,0x44,0x44,0x44,0x44,0x28,0x10,0x00}, // V
    {0x7C,0x40,0x40,0x78,0x40,0x40,0x7C,0x00}, // E
    {0x78,0x44,0x44,0x78,0x50,0x48,0x44,0x00}  // R
  };
  uint8_t frame[MATRIX_MODULES][8] = {};
  for (uint8_t letter = 0; letter < MATRIX_MODULES; ++letter)
    for (uint8_t y = 0; y < 8; ++y)
      frame[letter][y] = pgm_read_byte(violation ? &overGlyphs[letter][y] : &passGlyphs[letter][y]);
  writeMatrixFrame(frame);
}

// 숫자 0~9와 '-'의 3×7 글꼴. 각 줄의 하위 3비트가 왼쪽부터 세 점입니다.
static const uint8_t numberGlyphs[11][7] PROGMEM = {
  {7,5,5,5,5,5,7}, {2,6,2,2,2,2,7}, {7,1,1,7,4,4,7}, {7,1,1,7,1,1,7},
  {5,5,5,7,1,1,1}, {7,4,4,7,1,1,7}, {7,4,4,7,5,5,7}, {7,1,1,2,2,2,2},
  {7,5,5,7,5,5,7}, {7,5,5,7,1,1,7}, {0,0,0,7,0,0,0}
};

// 정수를 고정 배열에 씁니다. 동적 String/부동 소수점 문자열 포맷터가 필요하지 않습니다.
uint8_t formatUnsigned(uint32_t value, char *text) {
  char reversed[10];
  uint8_t count = 0;
  do { reversed[count++] = '0' + value % 10; value /= 10; } while (value);
  for (uint8_t i = 0; i < count; ++i) text[i] = reversed[count - 1 - i];
  text[count] = '\0';
  return count;
}

// 직전 속도 확인은 항상 소수 한 자리(cm/s)로 표시합니다. 예: 297 → "29.7".
void formatRecentSpeed() {
  uint8_t count = formatUnsigned(lastSpeedTenths / 10UL, overlayText);
  overlayText[count++] = '.';
  overlayText[count++] = '0' + lastSpeedTenths % 10UL;
  overlayText[count] = '\0';
}

void cancelOverlay() {
  overlayKind = OVERLAY_NONE;
  displayDirty = true;
}

// 문자열은 먼저 overlayText에 씁니다. 모든 숫자와 '.'/'-'만 사용합니다.
void beginOverlay(OverlayKind kind, uint32_t now, uint32_t duration) {
  overlayKind = kind;
  overlayStartedMs = now;
  overlayDurationMs = duration;
  overlayWidth = 0;
  for (uint8_t i = 0; overlayText[i]; ++i) overlayWidth += overlayText[i] == '.' ? 2 : 4;
  if (overlayWidth) --overlayWidth;  // 마지막 글자 뒤 여백은 제외합니다.
  overlayLastOffset = 255;
  displayDirty = true;
}

// 짧은 값은 가운데 정렬합니다. 긴 값은 잘리는 부분 없이 첫~마지막 자릿수를 순서대로 보여 줍니다.
void showNumericText(uint8_t offset) {
  uint8_t frame[MATRIX_MODULES][8] = {};
  int16_t cursor = overlayWidth <= 32 ? (32 - overlayWidth) / 2 : -(int16_t)offset;
  for (uint8_t i = 0; overlayText[i]; ++i) {
    char ch = overlayText[i];
    uint8_t width = ch == '.' ? 1 : 3;
    for (uint8_t y = 0; y < 7; ++y) {
      uint8_t row = ch == '.' ? (y == 6 ? 1 : 0) :
                    pgm_read_byte(&numberGlyphs[ch == '-' ? 10 : ch - '0'][y]);
      for (uint8_t x = 0; x < width; ++x) {
        int16_t screenX = cursor + x;
        if (screenX >= 0 && screenX < 32 && (row & (1U << (width - 1 - x))))
          frame[screenX / 8][y] |= 1U << (7 - screenX % 8);
      }
    }
    cursor += width + 1;
  }
  writeMatrixFrame(frame);
}

// 센서/부저와 독립된 표시 처리. 숫자 조회 중에도 결과의 유효 시간은 계속 흐릅니다.
void updateDisplay(uint32_t now) {
  if (overlayKind != OVERLAY_NONE && (uint32_t)(now - overlayStartedMs) >= overlayDurationMs)
    cancelOverlay();
  if (overlayKind != OVERLAY_NONE) {
    uint8_t offset = 0;
    if (overlayWidth > 32) {
      // 표시 시간의 앞/뒤 1/4은 끝을 고정해 읽게 하고 가운데 1/2 동안 이동합니다.
      uint32_t elapsed = now - overlayStartedMs;
      uint32_t hold = overlayDurationMs / 4;
      uint32_t travelTime = overlayDurationMs - 2 * hold;
      uint8_t travel = overlayWidth - 32;
      if (elapsed >= overlayDurationMs - hold) offset = travel;
      else if (elapsed > hold) offset = (uint64_t)(elapsed - hold) * travel / travelTime;
    }
    if (displayDirty || offset != overlayLastOffset) {
      showNumericText(offset);
      overlayLastOffset = offset;
    }
  } else if (measureState == RESULT) {
    bool wantedOn = !resultViolation || (((uint32_t)(now - resultStartedMs) / BLINK_HALF_MS) & 1UL) == 0;
    if (displayDirty || wantedOn != displayedOn) {
      if (wantedOn) showResultText(resultViolation);
      else matrixOff();
    }
  } else if (displayDirty || displayedOn) matrixOff();
  displayDirty = false;
}

// 읽기 전용 조회: 측정의 상태/시각이나 부저 타이머를 수정하지 않습니다.
void recallRecentSpeed(uint32_t now) {
  if (hasLastSpeed) {
    formatRecentSpeed();
    Serial.print(F("직전 속도 확인: "));
    Serial.print(overlayText);
    Serial.println(F(" 센티미터/초 (cm/s), 소수 한 자리 반올림"));
  } else {
    for (uint8_t i = 0; i < 4; ++i) overlayText[i] = '-';
    overlayText[4] = '\0';
    Serial.println(F("조회: 아직 유효한 측정 기록이 없습니다."));
  }
  beginOverlay(OVERLAY_RECALL, now, RECENT_DISPLAY_MS);
}

// 톤 발생기를 중지하고 구동 신호를 LOW로 내려 부저를 끕니다.
void stopBuzzer() {
  if (!ACTIVE_BUZZER) noTone(PIN_BUZZER);
  digitalWrite(PIN_BUZZER, LOW);
  buzzerOn = false;
}

// 부저를 시작하고 시각과 지속 시간만 저장합니다. 여기서 종료 시간을 기다리지 않습니다.
void startBuzzer(uint32_t now, uint32_t duration) {
  if (ACTIVE_BUZZER) digitalWrite(PIN_BUZZER, HIGH);
  // 수동 부저 음높이 변경 위치: 2000은 헤르츠입니다. 예를 들어 1500이면 더 낮은 음입니다.
  // 능동형 설정에서는 이 tone() 분기가 실행되지 않으므로 숫자를 바꿔도 음높이는 달라지지 않습니다.
  else tone(PIN_BUZZER, 2000);
  buzzerStartedMs = now;
  buzzerDurationMs = duration;
  buzzerOn = true;
}

// 앞 센서 HIGH→LOW 인터럽트: 첫 시각만 기록하여 센서 바운스가 덮어쓰지 못하게 합니다.
// 감지 횟수는 항상 기록합니다. ISR에서는 시리얼·표시·속도 계산·대기를 하지 않습니다.
void onSensorA() {
  ++sensorActivity;
  if (!captureEnabled || seenA) return;
  timestampA = micros();
  if (firstSensor == 0) firstSensor = digitalRead(PIN_SENSOR_B) == LOW ? 3 : 1;
  seenA = true;
}

// 뒤 센서 인터럽트: 첫 시각과 순서를 기록하고 처음부터 두 센서가 겹친 경우도 구분합니다.
void onSensorB() {
  ++sensorActivity;
  if (!captureEnabled || seenB) return;
  timestampB = micros();
  if (firstSensor == 0) firstSensor = digitalRead(PIN_SENSOR_A) == LOW ? 3 : 2;
  seenB = true;
}

// 공유 정보를 한 번에 복사합니다. ATOMIC_RESTORESTATE는 짧게 인터럽트를 막았다가 이전 상태로 복원합니다.
Capture readCapture() {
  Capture c;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    c.a = seenA; c.b = seenB;
    c.at = timestampA; c.bt = timestampB;
    c.first = firstSensor; c.activity = sensorActivity;
  }
  return c;
}

// 새 시각 기록을 금지합니다. 센서 감지 횟수는 계속 기록하여 비움 검사에 사용합니다.
void stopCapture() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { captureEnabled = false; }
}

// 현재 측정을 중단하고 두 센서가 연속해서 비워지기를 기다리는 상태로 돌아갑니다.
void waitForClear() {
  stopCapture();
  clearTiming = false;
  measureState = WAIT_CLEAR;
}

// 잘못된 측정을 한글로 알리고, 표시와 소리를 끈 뒤 비움 대기로 돌아갑니다.
void abortMeasurement(const __FlashStringHelper *reason) {
  waitForClear();
  cancelOverlay();
  matrixOff();
  stopBuzzer();
  Serial.print(F("측정 취소: "));
  Serial.println(reason);
}

// 버튼 접점 튐을 30밀리초 동안 걸러내고 떼었다가 새로 누른 경우에만 한 번 true를 반환합니다.
// 누른 채 유지하거나 부팅 때부터 누르고 있으면 반복 전환하지 않습니다.
bool freshButtonPress(uint32_t now) {
  uint8_t reading = digitalRead(PIN_RUN_BUTTON);
  if (reading != buttonRaw) { buttonRaw = reading; buttonChangedMs = now; }
  if (reading != buttonStable && (uint32_t)(now - buttonChangedMs) >= BUTTON_DEBOUNCE_MS) {
    buttonStable = reading;
    if (reading == HIGH) buttonReleased = true;
    else if (buttonReleased) { buttonReleased = false; return true; }
  }
  return false;
}

// 버튼의 새 누름에 작동 상태를 반전합니다. 먼저 진행 중인 측정·부저·표시를 정리합니다.
void toggleRunning() {
  running = !running;
  stopCapture();
  stopBuzzer();
  cancelOverlay();
  matrixOff();
  if (running) {
    waitForClear();
    Serial.println(F("작동 시작: 두 센서를 비워 주세요."));
  } else {
    measureState = STOPPED;
    Serial.println(F("작동 정지: 표시와 부저를 껐습니다."));
  }
}

// 부팅 시 실제 입력을 기준으로 초기화하여 눌린 채 부팅한 버튼은 무효로 둡니다.
void initializeActionButton(ActionButton &button, uint8_t pin, uint32_t now) {
  pinMode(pin, INPUT_PULLUP);
  button.raw = button.stable = digitalRead(pin);
  button.changedMs = now;
  button.ready = button.stable == HIGH;
}

// 반환값: 0=변화 없음, 1=유효한 새 누름, 2=안정적으로 뗌.
uint8_t updateActionButton(ActionButton &button, uint8_t reading, uint32_t now) {
  if (reading != button.raw) { button.raw = reading; button.changedMs = now; }
  if (reading == button.stable || (uint32_t)(now - button.changedMs) < BUTTON_DEBOUNCE_MS) return 0;
  button.stable = reading;
  if (reading == HIGH) { button.ready = true; return 2; }
  if (button.ready) { button.ready = false; return 1; }
  return 0;
}

void printCurrentLimit() {
  Serial.print(F("현재 제한 속도: "));
  Serial.print(speedLimitCmS);
  Serial.println(F(" 센티미터/초 (cm/s)"));
}

// 배포용 보조 입력은 D6 조회 버튼 하나뿐입니다. 고정 제한은 코드 설정으로만 바꿉니다.
void updateAuxiliaryButtons(uint32_t now, bool runChanged) {
  uint8_t reading = digitalRead(PIN_RECALL_BUTTON);
  uint8_t event = updateActionButton(recallButton, reading, now);
  bool down = reading == LOW || recallButton.stable == LOW;
  if (runChanged) auxiliaryBlocked = true;
  if (auxiliaryBlocked) {
    if (!down) auxiliaryBlocked = false;
    return;
  }
  if (event == 1) recallRecentSpeed(now);
}

// 두 입력이 HIGH인 상태가 100밀리초 유지되는지 봅니다. 짧은 감지도 횟수 변화로 검출합니다.
// 기록 허용 직전에는 원자 구간에서 입력을 다시 검사하여 상태 전환 중 경쟁을 줄입니다.
void updateClear(uint32_t now) {
  Capture c = readCapture();
  if (digitalRead(PIN_SENSOR_A) == LOW || digitalRead(PIN_SENSOR_B) == LOW) {
    clearTiming = false;
    return;
  }
  if (!clearTiming || c.activity != clearActivity) {
    clearTiming = true;
    clearStartedMs = now;
    clearActivity = c.activity;
    return;
  }
  if ((uint32_t)(now - clearStartedMs) < CLEAR_MS) return;
  bool armed = false;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    // 과거 대기 인터럽트 플래그를 지운 뒤 실제 입력과 감지 횟수를 재확인합니다.
    // 센서 비움 판정과 기록 시작 사이에 오래된 감지 신호가 섞이지 않게 합니다.
    EIFR = _BV(INTF0) | _BV(INTF1);
    if (sensorActivity == clearActivity && digitalRead(PIN_SENSOR_A) == HIGH &&
        digitalRead(PIN_SENSOR_B) == HIGH) {
      seenA = seenB = false;
      timestampA = timestampB = 0;
      firstSensor = 0;
      captureEnabled = true;
      armed = true;
    }
  }
  if (armed) { measureState = ARMED; Serial.println(F("측정 준비 완료")); }
  else clearTiming = false;
}

// 경과 시간을 검증하고 정수 비교로 정상/과속을 판정합니다. 표시·소리는 시작만 하고 즉시 돌아갑니다.
void finishMeasurement(const Capture &c) {
  // 카운터가 넘쳐 뒤 시각이 작아져도 unsigned 뺄셈은 올바른 짧은 경과 시간을 냅니다.
  uint32_t elapsedUs = (uint32_t)(c.bt - c.at);
  if (elapsedUs < MIN_INTERVAL_US) {
    abortMeasurement(F("동시 감지 또는 지나치게 짧은 감지 간격입니다."));
    return;
  }
  if (elapsedUs > TIMEOUT_US) {
    abortMeasurement(F("센서 사이 통과 시간이 오 초를 초과했습니다."));
    return;
  }
  stopCapture();
  // 속도(cm/s) = 거리(mm) × 100000 ÷ 경과 시간(마이크로초).
  // 1cm=10mm, 1초=1000000마이크로초이므로 계수는 100000입니다.
  // 판정은 64비트 정수 교차 곱을 사용하여 소수점 반올림 오차를 피합니다.
  // 제한도 cm/s 정수이므로 별도 1000배 단위 변환 없이 비교합니다.
  // 제한과 정확히 같으면 정상이고, 엄격하게 큰 경우만 과속입니다.
  resultViolation = (uint64_t)DISTANCE_MM * 100000ULL >
                    (uint64_t)speedLimitCmS * elapsedUs;
  // 소수 한 자리용 정수 반올림. 위의 정상/과속 판정과 독립적인 조회 기록입니다.
  // 거리 상한/최소 시간 설정에서도 계산 중간값은 64비트로 유지합니다.
  lastSpeedTenths = ((uint64_t)DISTANCE_MM * 1000000ULL + elapsedUs / 2UL) / elapsedUs;
  hasLastSpeed = true;
  cancelOverlay();  // 새 유효 결과가 직전 속도 확인/제한 숫자보다 우선합니다.
  resultStartedMs = millis();
  measureState = RESULT;
  showResultText(resultViolation);
  startBuzzer(resultStartedMs, resultViolation ? VIOLATION_BEEP_MS : PASS_BEEP_MS);
  // 출력은 소수 셋째 자리까지 반올림하지만 이 표시값으로 판정하지는 않습니다.
  // 예: 제한 30cm/s일 때 30.000으로 표시되어도 정확한 정수 비교 결과는 과속일 수 있습니다.
  Serial.print(resultViolation ? F("과속 | ") : F("정상 통과 | "));
  Serial.print(F("측정 속도: "));
  Serial.print((float)DISTANCE_MM * 100000.0f / (float)elapsedUs, 3);
  Serial.print(F(" 센티미터/초 (cm/s) | 제한 속도: "));
  Serial.print(speedLimitCmS);
  Serial.print(F(" 센티미터/초 (cm/s) | 경과 시간: "));
  Serial.print((float)elapsedUs / 1000.0f, 3);
  Serial.println(F(" 밀리초"));
}

// 겹침·역방향을 취소하거나 뒤 센서를 기다립니다. 두 시각이 있으면 결과를 계산하고 5초 초과를 검사합니다.
void processMeasurement() {
  Capture c = readCapture();
  if (c.first == 3) { abortMeasurement(F("두 센서가 동시에 감지되었습니다.")); return; }
  if (c.first == 2) { abortMeasurement(F("뒤 센서가 먼저 감지되었습니다. 진행 방향을 확인하세요.")); return; }
  if (!c.a) return;
  if (measureState == ARMED) {
    measureState = WAIT_B;
    Serial.println(F("앞 센서 감지: 뒤 센서를 기다립니다."));
  }
  if (c.b) { finishMeasurement(c); return; }
  if ((uint32_t)(micros() - c.at) > TIMEOUT_US) {
    // 첫 정보를 읽은 직후 뒤 센서가 감지될 수 있으므로 기록을 정지하면서 다시 읽습니다.
    // 시간초과 경계에 들어온 유효한 감지를 놓치지 않게 합니다.
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      captureEnabled = false;
      c.b = seenB; c.bt = timestampB;
    }
    if (c.b) finishMeasurement(c);
    else abortMeasurement(F("오 초 동안 뒤 센서 감지가 없어 종료했습니다."));
  }
}

// 핀·SPI·인터럽트를 초기화합니다. 부팅은 정지 상태이며 누른 채 켠 버튼은 무시합니다.
void setup() {
  Serial.begin(115200);
  pinMode(PIN_SENSOR_A, INPUT_PULLUP);
  pinMode(PIN_SENSOR_B, INPUT_PULLUP);
  pinMode(PIN_RUN_BUTTON, INPUT_PULLUP);
  uint32_t now = millis();
  initializeActionButton(recallButton, PIN_RECALL_BUTTON, now);
  
  buttonRaw = buttonStable = digitalRead(PIN_RUN_BUTTON);
  buttonReleased = buttonStable == HIGH;
  buttonChangedMs = now;
  digitalWrite(PIN_BUZZER, LOW);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_MATRIX_CS, HIGH);
  pinMode(PIN_MATRIX_CS, OUTPUT);
  pinMode(10, OUTPUT);  // CS를 옮겨도 우노 하드웨어 SPI SS는 출력이어야 합니다.
  SPI.begin();
  matrixOff();
  writeMatrixRegister(0x0F, 0);  // 전체 점등 시험 모드 해제.
  writeMatrixRegister(0x09, 0);
  writeMatrixRegister(0x0B, 7);
  writeMatrixRegister(0x0A, MATRIX_INTENSITY);
  attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_A), onSensorA, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_B), onSensorB, FALLING);
  Serial.println(F("속도계 준비 완료. 현재 정지 상태입니다. D4 버튼을 눌러 시작하세요."));
  Serial.print(F("센서 간격: "));
  Serial.print(DISTANCE_MM);
  Serial.println(F(" 밀리미터(mm)"));
  printCurrentLimit();
  Serial.println(F("D6: 직전 속도 확인, 마지막 유효 측정 속도를 소수 한 자리로 표시합니다. 기록은 재부팅 때 사라집니다."));
  Serial.println(F("배포용: 제한 속도는 코드의 고정값이며 버튼으로 변경하지 않습니다."));
}

// D4를 가장 먼저 처리합니다. 같은 반복의 조회/설정과 진행 중 누름은 무효로 합니다.
// 표시 처리는 정지 상태에서도 계속되어 D6/설정 숫자가 제시간에 꺼집니다.
void loop() {
  uint32_t now = millis();
  bool runChanged = freshButtonPress(now);
  if (runChanged) toggleRunning();
  updateAuxiliaryButtons(now, runChanged);
  if (runChanged) return;  // D4 전환이 확정된 반복에서는 다른 기능보다 소등이 우선입니다.
  now = millis();
  if (buzzerOn && (uint32_t)(now - buzzerStartedMs) >= buzzerDurationMs) stopBuzzer();
  if (running) {
    if (measureState == WAIT_CLEAR) updateClear(now);
    else if (measureState == ARMED || measureState == WAIT_B) processMeasurement();
    else if (measureState == RESULT && (uint32_t)(now - resultStartedMs) >= RESULT_MS) {
      waitForClear();
      displayDirty = true;  // 숫자 조회는 유지하되 원래 결과의 수명은 연장하지 않습니다.
    }
  }
  updateDisplay(millis());
}
