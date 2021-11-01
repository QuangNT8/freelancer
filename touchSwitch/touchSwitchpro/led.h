#include <ShiftRegister74HC595.h>
#define max_level 20

const int ledPin = LED_BUILTIN; // the number of the LED pin
int ledState = LOW;             // ledState used to set the LED

// parameters: <number of shift registers> (data pin, clock pin, latch pin)
ShiftRegister74HC595<3> sr(5, 7, 6);

uint32_t leds_registers = 0; // 0x00aaaa00;

void init_led_module()
{
    sr.setAllLow(); // set all pins LOW
}

void toggle_led(uint8_t idx)
{
    leds_registers ^= 1UL << idx;
}

void led_update()
{
    uint8_t *pinValues;
    pinValues = (uint8_t *)&leds_registers;
    sr.setAll(pinValues);
}

void set_led_on(uint8_t idx)
{
    leds_registers |= 1UL << idx;
    // sr.set(idx, HIGH); // set single pin HIGH
}

void set_led_off(int idx)
{
    leds_registers &= ~(1UL << idx);
    // sr.set(idx, LOW); // set single pin LOW
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

void update_led_level(uint8_t level1, uint8_t level2)
{
    uint8_t i = 0;

    for (i = 0; i < max_level; i++)
    {
        set_led_off(i);
    }

    for (i = 0; i < level1; i++)
    {
        set_led_on(i);
    }

    for (i = 10; i < 10 + level2; i++)
    {
        set_led_on(i);
    }
}