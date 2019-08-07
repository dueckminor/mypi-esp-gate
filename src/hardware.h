#ifndef __MYPI_ESP_DOOR_HARDWARE_H__
#define __MYPI_ESP_DOOR_HARDWARE_H__

extern void HardwareInitialize();
extern void HardwareWrite(bool bOn);
extern char HardwareRead();
extern void HardwareLED(bool bOn);

#endif/*__MYPI_ESP_DOOR_HARDWARE_H__*/