#include "timer.h"
#include "hardware.h"
#include <Arduino.h>

static unsigned long blinkPatternSOS = 
  1<<0  | 1<<2  | 1<<4 \
| 3<<7  | 3<<10 | 3<<13 \
| 1<<17 | 1<<19 | 1<<21;

static unsigned long blinkPatternOK = 1;

static unsigned long millisLoop = 0;
static unsigned long blinkPattern = 5;
static unsigned long blinkPatternNext = blinkPatternSOS;

unsigned long TimerLoop()
{
    millisLoop = millis();

    static unsigned long millisBegin = 0;
    if (millisLoop - millisBegin > 3000)
    {
        millisBegin = millisLoop;
        blinkPattern = blinkPatternNext;
    }

    unsigned long blinkPosition = (millisLoop - millisBegin)/100;

    HardwareLED((blinkPattern>>blinkPosition)&1);

    return millisLoop;
}

void TimerSetBlinkPattern(uint32_t _blinkPattern)
{
    blinkPatternNext = _blinkPattern;
}
void TimerSetBlinkPatternOK()
{
   blinkPatternNext = blinkPatternOK;
}
void TimerSetBlinkPatternSOS()
{
   blinkPatternNext = blinkPatternSOS;
}

Timer::Timer(unsigned long timeout)
{
    m_timeout = timeout;
    m_lastTrigger = millis();
}

bool Timer::Tick()
{
    if ((millisLoop - m_lastTrigger)>m_timeout)
    {
        m_lastTrigger = millisLoop;
        return true;
    }
    return false;
}