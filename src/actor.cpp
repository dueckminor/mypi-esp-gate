#include <Arduino.h>
#include "actor.h"
#include "hardware.h"
#include "debug.h"
#include "settings.h"

static bool s_bRequestOn = false;
static bool s_bRequestOff = false;

unsigned long millisOn = 0;
unsigned long millisOffIn = 0;

// ActorRelaisOff turns the relais off during next call of ActorLoopHandler
void ActorRelaisOff()
{
    s_bRequestOn = false;
    s_bRequestOff = true;
}

// ActorRelaisOn turns the relais on during next call of ActorLoopHandler
// It will be turned off 500 ms later
void ActorRelaisOn()
{
    s_bRequestOff = false;
    s_bRequestOn  = true;
}

// ActorLoopHandler sends the requested relais state to the hardware
void ActorLoopHandler()
{
    if (s_bRequestOn) 
    {
        s_bRequestOn = false;
        DebugDump("relais","1");
        HardwareWrite(true);
        millisOn = millis();
        millisOffIn = 500;
    }
    else if (s_bRequestOff)
    {
        s_bRequestOff = false;
        HardwareWrite(false);
        DebugDump("relais","0");
    }
    else if (millisOffIn > 0)
    {
        if ( (millis()-millisOn) >= millisOffIn )
        {
            HardwareWrite(false);
            DebugDump("relais","0");
            millisOn=millisOffIn=0;
        }
    }
}
