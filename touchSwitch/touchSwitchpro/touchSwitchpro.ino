#include "led.h"
#include "pwm.h"
#include "touchswitch.h"

void setup()
{
    // No pins to setup, pins can still be used regularly, although it will affect readings
    Serial.begin(9600);
    ref = init_touchSwitch();
    init_led_module();
    pinMode(ledPin, OUTPUT);
    init_timer_pwm();
}

uint8_t cnt_press[6];
uint8_t lever_group1 = 0, lever_group2 = 0;
void loop()
{
    int *bts;
    bts = read_adc_touch_bts(2);
    if (bts[0] == 1)
    {
        Serial.println("buton1 is pressed");
        set_led_on(0);
        if (pwm_duty1 < MAX_DUTY)
        {
            pwm_duty1++;
        }
    }
    else
    {
        set_led_off(0);
    }

    if (bts[1] == 1)
    {

        if (pwm_duty1 > 1)
        {
            pwm_duty1--;
        }
        Serial.println("buton2 is pressed");
    }

    if (bts[2] == 1)
    {

        if (cnt_press[2] <= 10)
        {
            if (cnt_press[2] == 0)
            {
                Serial.println("buton2 is pressed");
                if (lever_group1 < 10)
                {
                    lever_group1++;
                }
            }
            cnt_press[2]++;
        }
    }
    else
    {
        cnt_press[2] = 0;
    }

    if (bts[3] == 1)
    {
        Serial.println(cnt_press[4]);
        if (cnt_press[3] <= 10)
        {
            if (cnt_press[3] == 0)
            {
                Serial.println("buton3 is pressed");
                if (lever_group1 > 0)
                {
                    lever_group1--;
                }
            }
            cnt_press[3]++;
        }
    }
    else
    {
        cnt_press[3] = 0;
    }

    if (bts[4] == 1)
    {
        Serial.println(cnt_press[4]);
        if (cnt_press[4] <= 10)
        {
            if (cnt_press[4] == 0)
            {
                // Serial.println("buton5 is pressed");
                if (lever_group2 < 10)
                {
                    lever_group2++;
                }
            }
            cnt_press[4]++;
        }
    }
    else
    {
        cnt_press[4] = 0;
    }

    if (bts[5] == 1)
    {
        if (cnt_press[5] <= 10)
        {
            if (cnt_press[5] == 0)
            {
                // Serial.println("buton5 is pressed");
                if (lever_group2 > 0)
                {
                    lever_group2--;
                }
            }
            cnt_press[5]++;
        }
    }
    else
    {
        cnt_press[5] = 0;
    }

    update_led_level(lever_group1, lever_group2);
    led_update();
}
