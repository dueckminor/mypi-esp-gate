#ifndef __MYPI_ESP_GATE_TIMER_H__
#define __MYPI_ESP_GATE_TIMER_H__

extern unsigned long TimerLoop();

class Timer
{
public:
    Timer(unsigned long timeout);
    bool Tick();
protected:
    unsigned long m_timeout;
    unsigned long m_lastTrigger;
};

#endif/*__MYPI_ESP_GATE_TIMER_H__*/
