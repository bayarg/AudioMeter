#define NUM_LED (10)
#define PEAK_HOLD_TIME (80)  // 80 카운트 (약 1초) 동안 Peak 값 유지
#define BLINK_INTERVAL (500) // LED 점멸 주기 (ms)

volatile uint16_t adcCounter = 0;
volatile uint16_t adcValue = 0;
volatile bool adcReady = false;

// Peak Hold 관련 변수
int currentLevel = 0;      // 현재 소음 레벨
int peakLevel = 0;         // Peak 레벨
int peakCounter = 0;       // Peak 유지 시간 카운터

// LED 점멸 관련 변수
unsigned long lastBlinkTime = 0;
bool blinkState = false;

void setup() {
    Serial.begin(115200);

    // LED 핀 모드 설정
    for (int i = 2; i < NUM_LED + 2; i++) {
        pinMode(i, OUTPUT);
    }

    // ADC 설정
    ADMUX = 0;
    ADMUX |= (1 << REFS0);  // AVCC 기준 전압 사용
    ADMUX |= 0;             // ADC0 채널 선택

    ADCSRA = 0;
    ADCSRA |= (1 << ADEN);   // ADC 활성화
    ADCSRA |= (1 << ADIE);   // ADC 인터럽트 활성화
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0); // 분주비 128

    sei(); // 전역 인터럽트 활성화

    ADCSRA |= (1 << ADSC); // ADC 변환 시작
}

void loop() {
    if (adcReady) {
        // 현재 레벨 계산 (0 ~ 11)
        currentLevel = map(adcValue, 0, 200, 0, NUM_LED + 1);
        currentLevel = constrain(currentLevel, 0, 11);

        // Peak Hold 알고리즘
        if (currentLevel > peakLevel) {
            // 현재 값이 Peak보다 크면 즉시 갱신
            peakLevel = currentLevel;
            peakCounter = 0;
        } else {
            // Peak 유지 시간 카운트
            peakCounter++;
            
            // 일정 시간 경과 후 Peak 값 감소
            if (peakCounter >= PEAK_HOLD_TIME) {
                peakCounter = 0;
                if (peakLevel > 0) {
                    peakLevel--; // 1단계씩 부드럽게 감소
                }
            }
        }

        // 디버그 출력
        Serial.print("Current: ");
        Serial.print(currentLevel);
        Serial.print(" | Peak: ");
        Serial.print(peakLevel);
        Serial.print(" | ADC: ");
        Serial.println(adcValue);

        adcReady = false;
    }

    // LED 출력 처리
    updateLED();
}

void updateLED() {
    // 위험 레벨(11) 도달 시 점멸 모드
    if (peakLevel >= 11) {
        unsigned long currentTime = millis();
        
        // 점멸 주기 확인
        if (currentTime - lastBlinkTime >= BLINK_INTERVAL) {
            lastBlinkTime = currentTime;
            blinkState = !blinkState;
            
            // 모든 LED 점멸
            for (int i = 2; i < NUM_LED + 2; i++) {
                digitalWrite(i, blinkState ? HIGH : LOW);
            }
        }
    } else {
        // 정상 모드: Peak 값에 따라 LED 점등
        for (int i = 2; i < NUM_LED + 2; i++) {
            if (i < peakLevel + 2) {
                digitalWrite(i, HIGH);
            } else {
                digitalWrite(i, LOW);
            }
        }
    }
}

// ADC 변환 완료 인터럽트
ISR(ADC_vect) {
    if (++adcCounter == 1500) {  // 약 80Hz 샘플링
        adcCounter = 0;
        adcValue = ADC;
        adcReady = true;
    }
    ADCSRA |= (1 << ADSC); // 다음 변환 시작
}