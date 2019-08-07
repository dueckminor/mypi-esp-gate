#include <Arduino.h>
#include "actor.h"
#include "hardware.h"

unsigned long millisOn = 0;
unsigned long millisOffIn = 0;

void ActorRelaisOff()
{
    HardwareWrite(false);
}

void ActorRelaisOn()
{
    HardwareWrite(true);
    millisOn = millis();
    millisOffIn = 500;
}

void ActorLoop()
{
    if (millisOffIn > 0)
    {
        if ( (millis()-millisOn) >= millisOffIn )
        {
            ActorRelaisOff();
            millisOn=millisOffIn=0;
        }
    }
}
