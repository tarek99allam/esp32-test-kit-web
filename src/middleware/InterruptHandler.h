#ifndef INTERRUPTHANDLER_H
#define INTERRUPTHANDLER_H

#include <Arduino.h>

class InterruptHandler
{
  public:
    InterruptHandler(int pin);
    void begin();
    bool checkInterrupt();

  private:
    int _pin;
    static volatile bool _interruptFlag; // Make the flag static so it can be used in the ISR
    static void IRAM_ATTR handleInterrupt();
};

#endif // INTERRUPTHANDLER_H
