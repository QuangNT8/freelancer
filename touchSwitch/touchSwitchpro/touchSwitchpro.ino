#include <ShiftRegister74HC595.h>
#include <ADCTouch.h>
#define refer_val 500
#define threshold 60

#define PWM1_PIN 8
#define PWM2_PIN 9
#define MAX_DUTY 50

const int ledPin =  LED_BUILTIN;  // the number of the LED pin
int ledState = LOW;             // ledState used to set the LED

int *ref;//reference values to remove offset

// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<3> sr(5, 7, 6);
  
static int* init_touchSwitch()
{
  static int ref[6];
  
  ref[0] = ADCTouch.read(A0, refer_val);    //create reference values to 
  ref[1] = ADCTouch.read(A1, refer_val);    //account for the capacitance of the pad
  ref[2] = ADCTouch.read(A2, refer_val);    //account for the capacitance of the pad
  ref[3] = ADCTouch.read(A3, refer_val);    //account for the capacitance of the pad
  ref[4] = ADCTouch.read(A4, refer_val);    //account for the capacitance of the pad
  ref[5] = ADCTouch.read(A5, refer_val);    //account for the capacitance of the pad
  
  return &ref[0];
}

void toggle_led(int idx)
{
  static int flag_toggle=0;

  if(flag_toggle != 0)
  {
    sr.set(idx, HIGH); // set single pin HIGH
    flag_toggle = 0;
  }
  else
  {
    sr.set(idx, LOW); // set single pin LOW
    flag_toggle = 1;
  }
  
}

void init_timer()
{
  // initialize Timer1
  noInterrupts(); // disable all interrupts
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;
  OCR1A = 400;  //31250/2/1000;  // compare match register 16MHz/256/2Hz
  TCCR1B |= (1 << WGM12);   // CTC mode
  TCCR1B |= ((1 << CS10));    // 256 prescaler
  TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  interrupts();             // enable all interrupts
}

uint8_t cnt=0,cnt_on=0,cnt_off=0;
int pwm1_state = LOW;
int pwm2_state = LOW;
uint8_t pwm_duty1=10;
uint8_t pwm_duty2=5;

ISR(TIMER1_COMPA_vect) // timer compare interrupt service routine
{ 
  if(cnt < MAX_DUTY)
  {
    cnt++;
  } 
  else
  {
      cnt = 0;
  }

  if(cnt<pwm_duty1)
  {
    pwm1_state = HIGH;
  } 
  else 
  {
    pwm1_state = LOW;
  }

  if(cnt<pwm_duty2)
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

void init_led_module()
{
  sr.setAllLow(); // set all pins LOW
  //uint8_t pinValues[] = {0xAA, 0xAA, 0xAA}; 
  //sr.setAll(pinValues); 
}

void set_led_on(int idx)
{
  sr.set(idx, HIGH); // set single pin HIGH
}

void set_led_off(int idx)
{
  sr.set(idx, LOW); // set single pin LOW
}

void set_all_led_off(int idx)
{
  uint8_t pinValues[] = {0x00, 0x00, 0x00}; 
  sr.setAll(pinValues); 
}

void set_all_led_on(int idx)
{
  uint8_t pinValues[] = {0xff, 0xff, 0xff}; 
  sr.setAll(pinValues); 
}

void setup() 
{
  // No pins to setup, pins can still be used regularly, although it will affect readings
  Serial.begin(9600);
  ref = init_touchSwitch();
  init_led_module();
  pinMode(ledPin, OUTPUT);
  init_timer();
} 


int *read_adc_touch_bts(int debug_mode)
{
  static int st_bt[6];
  int value0 = ADCTouch.read(A0); 
  int value1 = ADCTouch.read(A1);  
  int value2 = ADCTouch.read(A2);
  int value3 = ADCTouch.read(A3);
  int value4 = ADCTouch.read(A4);
  int value5 = ADCTouch.read(A5);

  value0 -= ref[0];       //remove offset
  value1 -= ref[1];
  value2 -= ref[2];
  value3 -= ref[3];
  value4 -= ref[4];
  value5 -= ref[5];
  
  st_bt[0] = value0 > threshold;
  st_bt[1] = value1 > threshold;
  st_bt[2] = value2 > threshold;
  st_bt[3] = value3 > threshold;
  st_bt[4] = value4 > threshold;
  st_bt[5] = value5 > threshold;

  switch(debug_mode){
    case 1:
    Serial.print(st_bt[0]);    
    Serial.print("\t");      
    
    Serial.print(st_bt[1]);
    Serial.print("\t\t");
    
    Serial.print(st_bt[2]);
    Serial.print("\t\t");
    
    Serial.print(st_bt[3]);
    Serial.print("\t\t");
   
    Serial.print(st_bt[4]);
    Serial.print("\t\t");
   
    Serial.println(st_bt[5]);
    
    break;
    case 2:
    Serial.print(value0);   
    Serial.print("\t");
    Serial.print(value1);   
    Serial.print("\t");
    Serial.print(value2);   
    Serial.print("\t");
    Serial.print(value3);   
    Serial.print("\t");
    Serial.print(value4);   
    Serial.print("\t");
    Serial.println(value5);
    break;
  }
 
  return &st_bt[0];
}

void loop() 
{
    int *bts;
    bts = read_adc_touch_bts(2);
    if(bts[0]==1)
    {
      Serial.println("buton1 is pressed");
      set_led_on(0);
      pwm_duty1++;
    }else
    {
      set_led_off(0);
    }

   if(bts[1]==1)
   {
     pwm_duty1--;
     if (pwm_duty1 == 0)
     {
        pwm_duty1 = MAX_DUTY;
     }
     Serial.println("buton2 is pressed");
   }
   if(bts[2]==1)
   {
     Serial.println("buton3 is pressed");
     if (pwm_duty2 < MAX_DUTY)
     {
        pwm_duty2++;
        //pwm_duty2 = MAX_DUTY;
     }
   }
   if(bts[3]==1)
   {
     Serial.println("buton4 is pressed");
     
     if (pwm_duty2 > 1)
     {
        pwm_duty2--;
        //pwm_duty2 = MAX_DUTY;
     }
   }
   if(bts[4]==1)
   {
     Serial.println("buton5 is pressed");
   }
   if(bts[5]==1)
   {
     Serial.println("buton6 is pressed");
   }
   
}
