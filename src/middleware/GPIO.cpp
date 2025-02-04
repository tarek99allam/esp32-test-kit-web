#include "GPIO.h"
void init_gpio()
{
    pinMode(DC_TOGGLE_LINE1, OUTPUT);
    pinMode(DC_TOGGLE_LINE2, OUTPUT);

    pinMode(OUTPUT_RELE1, INPUT);
    pinMode(OUTPUT_RELE2, INPUT);
}