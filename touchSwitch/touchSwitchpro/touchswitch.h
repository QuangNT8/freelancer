#include <ADCTouch.h>

#define refer_val 50

#define threshold0 100
#define threshold1 100
#define threshold2 60
#define threshold3 60
#define threshold4 60
#define threshold5 60

int *ref; // reference values to remove offset

static int *init_touchSwitch()
{
    static int ref[6];

    ref[0] = ADCTouch.read(A0, refer_val);
    ref[1] = ADCTouch.read(A1, refer_val);
    ref[2] = ADCTouch.read(A2, refer_val);
    ref[3] = ADCTouch.read(A3, refer_val);
    ref[4] = ADCTouch.read(A4, refer_val);
    ref[5] = ADCTouch.read(A5, refer_val);

    return &ref[0];
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

    value0 -= ref[0]; // remove offset
    value1 -= ref[1];
    value2 -= ref[2];
    value3 -= ref[3];
    value4 -= ref[4];
    value5 -= ref[5];

    st_bt[0] = value0 > threshold0;
    st_bt[1] = value1 > threshold1;
    st_bt[2] = value2 > threshold2;
    st_bt[3] = value3 > threshold3;
    st_bt[4] = value4 > threshold4;
    st_bt[5] = value5 > threshold5;

    switch (debug_mode)
    {
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