#include <ShiftRegister74HC595.h>
#include <ADCTouch.h>
#define refer_val 500
#define threshold 200

int *ref;//reference values to remove offset
// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<3> sr(0, 1, 2);
  
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

void init_led_module()
{
  sr.setAllLow(); // set all pins LOW
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
    }
    if(bts[1]==1)
    {
      Serial.println("buton2 is pressed");
    }
    if(bts[2]==1)
    {
      Serial.println("buton3 is pressed");
    }
    if(bts[3]==1)
    {
      Serial.println("buton4 is pressed");
    }
    if(bts[4]==1)
    {
      Serial.println("buton5 is pressed");
    }
    if(bts[5]==1)
    {
      Serial.println("buton6 is pressed");
    }
    delay(10);
}