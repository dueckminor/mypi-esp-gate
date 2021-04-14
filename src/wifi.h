#ifndef __MYPI_ESP_GATE_WIFI_H__
#define __MYPI_ESP_GATE_WIFI_H__

#include <ESP8266WiFi.h>

extern void WifiInitialize();
extern void WifiInitializeWPS();
extern bool WifiIsConnected();
extern WiFiClient &WifiGetClient();

// WifiSetupLoop returns true if connected
extern bool WifiSetupLoop();

// WifiLoop returns true if connected
extern bool WifiLoop();

#endif /*__MYPI_ESP_GATE_WIFI_H__*/
