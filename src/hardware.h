#ifndef __MYPI_ESP_GATE_HARDWARE_H__
#define __MYPI_ESP_GATE_HARDWARE_H__

extern void HardwareInitialize();
extern void HardwareLoop();

extern void HardwareWrite(bool bOn);
extern char HardwareRead();
extern bool HardwareReadWPS();
extern void HardwareLED(bool bOn);
extern void HardwareDimLED(int value);

extern unsigned char HardwareHaveEvents();

#endif /*__MYPI_ESP_GATE_HARDWARE_H__*/