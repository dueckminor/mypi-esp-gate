#include "wifi.h"
#include "settings.h"
#include "debug.h"
#include "timer.h"

WiFiClientSecure espClient;

const char *ssid = MYPI_SSID;
const char *password = MYPI_PSK;

void WifiInitialize()
{
  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.hostname("esp-tor-2");

  // int nNetworks = WiFi.scanNetworks(false, false, 0, (uint8 *)ssid);
  // Serial.printf("network count: %d\n", nNetworks);
  // int32_t rssi = -1000;
  // uint8_t *bssid = NULL;
  // for (int i = 0; i < nNetworks; i++)
  // {
  //   int32_t rssiThis = WiFi.RSSI(i);
  //   Serial.printf("- %s (%s) %d dBm\n", WiFi.BSSIDstr(i).c_str(), WiFi.SSID(i).c_str(), rssiThis);
  //   if (rssiThis > rssi)
  //   {
  //     bssid = WiFi.BSSID(i);
  //     rssi = rssiThis;
  //   }
  // }

  // WiFi.begin(ssid, password, 0, bssid);

  static BearSSL::X509List trust((const uint8_t *)MYPI_CA_CERT, sizeof(MYPI_CA_CERT));
  espClient.setTrustAnchors(&trust);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
  }
}

bool WifiIsConnected()
{
  return WiFi.status() == WL_CONNECTED;
}

void WifiOnComplete(int nNetworks)
{
  Serial.print("nNetworks:");
  Serial.println(nNetworks);

  int32_t rssi = -1000;
  uint8_t *bssid = NULL;
  int iBestNetwork = -1;
  for (int i = 0; i < nNetworks; i++)
  {
    int32_t rssiThis = WiFi.RSSI(i);
    if (WiFi.SSID(i) != ssid)
    {
      continue;
    }

    if (rssiThis > rssi)
    {
      bssid = WiFi.BSSID(i);
      rssi = rssiThis;
      iBestNetwork = i;
    }
  }

  String bestBSSID = WiFi.BSSIDstr(iBestNetwork);
  String usedBSSID = WiFi.BSSIDstr();

  char dump[100];
  sprintf(dump, "%s (%s) %d dBm", bestBSSID.c_str(), WiFi.SSID(iBestNetwork).c_str(), rssi);

  Serial.printf("BEST: %s\n", dump);
  DebugDump("wifi", dump);
  if (!WiFi.isConnected() || (usedBSSID != bestBSSID))
  {
    Serial.printf("USED: %s (%s) %d dBm\n", usedBSSID.c_str(), WiFi.SSID().c_str(), WiFi.RSSI());
    WiFi.begin(ssid, password, 0, bssid);
    while (WiFi.status() != WL_CONNECTED)
    {
      delay(500);
    }
  }
}



bool WifiLoop()
{
  if (!WiFi.isConnected())
  {
    Serial.println("not connected");
    WiFi.scanNetworksAsync(&WifiOnComplete);
    return false;
  }

  static Timer timer(5000);
  if (timer.Tick())
  {
    long rssi = WiFi.RSSI();
    char msg[128];
    snprintf(msg,sizeof(msg),"{\"rssi\":%i,\"ssid\":\"%s\",\"bssid\":\"%s\"}",(int)rssi,WiFi.SSID().c_str(),WiFi.BSSIDstr().c_str());
    DebugDump("wifi",msg);
    if (rssi< -100)
    {
      WiFi.scanNetworksAsync(&WifiOnComplete);
    }
  }

  return true;
}

WiFiClient &WifiGetClient()
{
  return espClient;
}