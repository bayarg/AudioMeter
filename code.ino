#define NUM_LED (10)
volatile uint16_t adcCounter = 0;
volatile uint16_t adcValue = 0;
volatile bool adcReady = false;

void setup() {
    Serial.begin(115200);

    pinMode(2, OUTPUT);
    pinMode(3, OUTPUT);
    pinMode(4, OUTPUT);
    pinMode(5, OUTPUT);
    pinMode(6, OUTPUT);
    pinMode(7, OUTPUT);
    pinMode(8, OUTPUT);
    pinMode(9, OUTPUT);
    pinMode(10, OUTPUT);
    pinMode(11, OUTPUT);


    ADMUX = 0;
    ADMUX |= (1 << REFS0);
    ADMUX |= 0; 

    ADCSRA = 0;
    ADCSRA |= (1 << ADEN);
    ADCSRA |= (1 << ADIE);
    ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);

    sei();

    ADCSRA |= (1 << ADSC);
}

void loop() {
    if (adcReady) {

        // 레벨 매핑 - begin
        int value = map(adcValue, 0, 200, 0, NUM_LED + 1);
        value = min(11, value);
        // 레벨 매핑 - end

        // LED 출력 - begin
        for (int i = 2; i < NUM_LED+2; i++) {
            digitalWrite(i, 0);
        }
        for (int i = 2; i < value+2; i++) {
            digitalWrite(i, 1);
        }
        // LED 출력 - end

        adcReady = false;
    }
}

// 신호 샘플링 - begin
ISR(ADC_vect) {
    if (++adcCounter == 1562) { // 1562로 설정한 이유는 목표하는 제어 주기인 80Hz로 다운 샘플링 하고자함.
        adcCounter = 0;
        adcValue = ADC;
        adcReady = true;
    }
    ADCSRA |= (1 << ADSC);
}
// 신호 샘플링 - end