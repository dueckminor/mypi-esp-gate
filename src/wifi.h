#ifndef __MYPI_ESP_DOOR_WIFI_H__
#define __MYPI_ESP_DOOR_WIFI_H__

#include <ESP8266WiFi.h>

extern void WifiInitialize();
extern WiFiClient& WifiGetClient();

#endif/*__MYPI_ESP_DOOR_WIFI_H__*/
