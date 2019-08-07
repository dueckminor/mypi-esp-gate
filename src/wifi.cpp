#include "wifi.h"
#include "settings.h"

WiFiClientSecure espClient;

const char* ssid = MYPI_SSID;
const char* password = MYPI_PSK;

void WifiInitialize()
{
  // Connect to WiFi network
  WiFi.mode(WIFI_STA);
  WiFi.hostname("esp-tor");
  WiFi.begin(ssid, password);

  static BearSSL::X509List trust((const uint8_t*)MYPI_CA_CERT,sizeof(MYPI_CA_CERT));
  espClient.setTrustAnchors(&trust);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }

}

WiFiClient& WifiGetClient()
{
    return espClient;
}