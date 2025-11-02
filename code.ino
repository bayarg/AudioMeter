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
        int value = map(adcValue, 0, 200, 0, NUM_LED + 1);
        value = min(11, value);
        for (int i = 2; i < NUM_LED+2; i++) {
            digitalWrite(i, 0);
        }
        for (int i = 2; i < value+2; i++) {
            digitalWrite(i, 1);
        }
        adcReady = false;
    }
}

ISR(ADC_vect) {
    if (++adcCounter == 1500) {
        adcCounter = 0;
        adcValue = ADC;
        adcReady = true;
    }
    ADCSRA |= (1 << ADSC);
}