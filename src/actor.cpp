#include <Arduino.h>
#include "actor.h"
#include "hardware.h"
#include "mqtt.h"
#include "settings.h"

unsigned long millisOn = 0;
unsigned long millisOffIn = 0;

void ActorRelaisOff()
{
    MqttPublish(MYPI_TOR_ID "/debug/relais","0");
    HardwareWrite(false);
}

void ActorRelaisOn()
{
    MqttPublish(MYPI_TOR_ID "/debug/relais","1");
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
