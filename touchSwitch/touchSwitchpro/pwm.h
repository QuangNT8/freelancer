#define PWM1_PIN 8
#define PWM2_PIN 9
#define MAX_DUTY 50

uint8_t cnt = 0, cnt_on = 0, cnt_off = 0;
int pwm1_state = LOW;
int pwm2_state = LOW;
uint8_t pwm_duty1 = 10;
uint8_t pwm_duty2 = 10;

void init_timer_pwm()
{
    // initialize Timer1
    noInterrupts(); // disable all interrupts
    TCCR1A = 0;
    TCCR1B = 0;
    TCNT1 = 0;
    OCR1A = 400;             // 31250/2/1000;  // compare match register 16MHz/256/2Hz
    TCCR1B |= (1 << WGM12);  // CTC mode
    TCCR1B |= ((1 << CS10)); // 256 prescaler
    TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
    interrupts();            // enable all interrupts
}

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{
    if (cnt < MAX_DUTY)
    {
        cnt++;
    }
    else
    {
        cnt = 0;
    }

    if (cnt < pwm_duty1)
    {
        pwm1_state = HIGH;
    }
    else
    {
        pwm1_state = LOW;
    }

    if (cnt < pwm_duty2)
    {
        pwm2_state = HIGH;
    }
    else
    {
        pwm2_state = LOW;
    }

    // digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    digitalWrite(PWM1_PIN, pwm1_state);
    digitalWrite(PWM2_PIN, pwm2_state);
}