#include <Arduino.h>
#include "hardware.h"
#include <Wire.h>
#include "settings.h"

const char IC_8574 = 32;
char IC_8574_OUTPUT = 31;

#define LED_ON_BOARD 1 // ESP-01

void ic8574Write() {
    Wire.beginTransmission(IC_8574);
    Wire.write(~IC_8574_OUTPUT);
    Wire.endTransmission();
}

void HardwareInitialize()
{
    // prepare LED
#ifdef USE_STATUS_LED
    pinMode(LED_ON_BOARD, OUTPUT);
    digitalWrite(LED_ON_BOARD, LOW);
#endif

    Wire.begin(2,0);
    Wire.setClock(400000L);
    Wire.beginTransmission(IC_8574);
    Wire.write(~IC_8574_OUTPUT);
    Wire.endTransmission();
}

void HardwareWrite(bool bOn)
{
    if (bOn)
    {
        bitSet(IC_8574_OUTPUT,7);
    }
    else
    {
        bitClear(IC_8574_OUTPUT,7);
    }
    ic8574Write();
}

char HardwareRead()
{
    Wire.requestFrom(IC_8574,1);
    return Wire.read() & 0x1f;
}

void HardwareLED(bool bOn)
{
#ifdef USE_STATUS_LED
    digitalWrite(LED_ON_BOARD, bOn ? 1 : 0);
#endif
}