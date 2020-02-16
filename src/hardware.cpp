#include <Arduino.h>
#include "hardware.h"
#include <Wire.h>
#include "settings.h"

const char IC_8574 = 32;
char IC_8574_OUTPUT = 0x80 + 0x1f;

#if 0
#define USE_8574
#define LED_ON_BOARD 1 // ESP-01
#define PIN_SDA 2
#define PIN_SCL 3
#else
// WEMOS D1 Mini

// D0 = GPIO16   builtin-pull-down  X
// D1 = GPIO5    releais
// D2 = GPIO4                       X
// D3 = GPIO0    boot mode
// D4 = GPIO2    boot mode + led
// D5 = GPIO14                      X
// D6 = GPIO12                      X
// D7 = GPIO13                      X
// D8 = GPIO15   boot mode

#define LED_ON_BOARD D4
#define PIN_RELAIS D1

#define PIN_SENSOR_0 D2
#define PIN_SENSOR_1 D0
#define PIN_SENSOR_2 D5
#define PIN_SENSOR_3 D6
#define PIN_SENSOR_4 D7
#endif

#ifdef USE_8574
static unsigned char bits = 0;
static unsigned char cEvents = 0;
#else
static volatile unsigned char aBits[256];
static volatile unsigned char writePos=0;
static volatile unsigned char readPos=0;
static volatile unsigned char cEvents=0;

ICACHE_RAM_ATTR
static void change()
{
    static unsigned char lastBits = 0xff;
    unsigned char bits = 0;
    bits |= (digitalRead(PIN_SENSOR_0) ? 1 : 0) << 0;
    bits |= (digitalRead(PIN_SENSOR_1) ? 1 : 0) << 1;
    bits |= (digitalRead(PIN_SENSOR_2) ? 1 : 0) << 2;
    bits |= (digitalRead(PIN_SENSOR_3) ? 1 : 0) << 3;
    bits |= (digitalRead(PIN_SENSOR_4) ? 1 : 0) << 4;
    if (bits == lastBits)
    {
        return;
    }
    lastBits = bits;
    aBits[writePos++] = bits;
    if (cEvents==0xff) {
        readPos++;
    }
    else
    {
        cEvents++;
    }
}
#endif

void HardwareInitialize()
{
    // prepare LED
#ifdef USE_STATUS_LED
    pinMode(LED_ON_BOARD, OUTPUT);
    digitalWrite(LED_ON_BOARD, LOW);
#endif

#ifdef USE_8574
    Wire.begin(2, 0);
    Wire.setClock(400000L);

    // Wire.beginTransmission(IC_8574);
    // Wire.write(IC_8574_OUTPUT);
    // Wire.endTransmission();

    Wire.beginTransmission(IC_8574);
    Wire.write(IC_8574_OUTPUT);
    Wire.endTransmission();
#else
    memset((void*)aBits,0xff,sizeof(aBits));

    pinMode(PIN_RELAIS, OUTPUT);
    pinMode(PIN_SENSOR_0, INPUT_PULLUP);
    pinMode(PIN_SENSOR_1, INPUT);
    pinMode(PIN_SENSOR_2, INPUT_PULLUP);
    pinMode(PIN_SENSOR_3, INPUT_PULLUP);
    pinMode(PIN_SENSOR_4, INPUT_PULLUP);

    attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_0),change,CHANGE);
    // attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_1),change,CHANGE);
    // attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_2),change,CHANGE);
    // attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_3),change,CHANGE);
    attachInterrupt(digitalPinToInterrupt(PIN_SENSOR_4),change,CHANGE);
    change();
#endif
}

void HardwareLoop()
{
#ifdef USE_8574
    char bitsRead = -1;
    Wire.requestFrom(IC_8574, 1);
    if (Wire.available())
    {
        bitsRead = Wire.read();
    }
    bitsRead &= 0x1f;
    if (bits != bitsRead)
    {
        bits = bitsRead;
        cEvents = 1;
    }
#endif
}

void HardwareWrite(bool bOn)
{
#ifdef USE_8574
    if (bOn)
    {
        bitClear(IC_8574_OUTPUT, 7);
    }
    else
    {
        bitSet(IC_8574_OUTPUT, 7);
    }

    Wire.beginTransmission(IC_8574);
    Wire.write(IC_8574_OUTPUT);
    Wire.endTransmission();
#else
    digitalWrite(PIN_RELAIS, bOn ? HIGH : LOW);
#endif
}

unsigned char HardwareHaveEvents()
{
    return cEvents;
}

char HardwareRead()
{
#ifdef USE_8574
    static char bits = -1;
    Wire.requestFrom(IC_8574, 1);
    if (Wire.available())
    {
        bits = Wire.read();
    }
    return bits & 0x1f;
#else
    volatile unsigned char bits = 0xff;
    cli();
    bits = aBits[readPos];
    if (cEvents)
    {
        readPos++;
        cEvents--;
    }
    sei();
    return bits;
#endif
}

void HardwareLED(bool bOn)
{
#ifdef USE_STATUS_LED
    digitalWrite(LED_ON_BOARD, bOn ? 0 : 1);
#endif
}

void HardwareDimLED(int value)
{
    analogWrite(LED_ON_BOARD, value);
}
