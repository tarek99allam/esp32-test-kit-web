#include "InterruptHandler.h"
#include "../defines.h"

volatile bool InterruptHandler::_interruptFlag = false; // Define the static variable

InterruptHandler::InterruptHandler(int pin) : _pin(pin)
{
}

void InterruptHandler::begin()
{
    pinMode(_pin, INPUT_PULLUP); // Set pin as input with internal pull-up resistor
    attachInterrupt(digitalPinToInterrupt(_pin), handleInterrupt, FALLING); // Attach ISR to pin
}

bool InterruptHandler::checkInterrupt()
{
    if (_interruptFlag)
    {
        _interruptFlag = false; // Reset flag after handling the interrupt
        return true;
    }
    return false;
}

// Static ISR function to set the interrupt flag
void IRAM_ATTR InterruptHandler::handleInterrupt()
{
    _interruptFlag = true; // Modify the static interrupt flag
}
