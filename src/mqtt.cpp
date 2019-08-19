#include <Arduino.h>
#include <PubSubClient.h>
#include "mqtt.h"
#include "wifi.h"
#include "settings.h"
#include "sensor.h"
#include "actor.h"
#include "gate.h"

const char* mqtt_server = MYPI_HOST;

static PubSubClient mqttClient(WifiGetClient());
static void mqttCallback(char* topic, byte* payload, unsigned int length);

void MqttInitialize()
{
    mqttClient.setServer(mqtt_server, 8883);
    mqttClient.setCallback(mqttCallback);
}

static void mqttCallback(char* topic, byte* payload, unsigned int length) {
  // Switch on the LED if an 1 was received as first character
  if (0 == strcmp(topic, MYPI_TOR_ID "/debug/setbits")) {
    char buffer[5];
    if (length>4) {
      return;
    }
    memcpy(buffer,payload,length);
    buffer[length]='\0';
    SensorAnalyseDebugInput(atoi(buffer));
    return;
  }
  else if (0 == strcmp(topic, MYPI_TOR_ID "/relais")) {
    if ((char)payload[0] == '1') {
      ActorRelaisOn();
    } else if ((char)payload[0] == '0') {
      ActorRelaisOff();
    }
  }
  else if (0 == strcmp(topic, MYPI_TOR_ID "/open")) {
    GateRequestPosition(GatePosition_Open);
  }
  else if (0 == strcmp(topic, MYPI_TOR_ID "/close")) {
    GateRequestPosition(GatePosition_Closed); 
  }
}

bool mqttLoopConnected = false;

void MqttEnsureConnected();
boolean MqttPublish(const char* topic, const char* payload);

void MqttBeginLoop() {
  mqttLoopConnected = false;
}

void mqttReconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    // Create a random client ID
    String clientId = "ESP01-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (mqttClient.connect(clientId.c_str())) {
      // Once connected, publish an announcement...
      MqttPublish(MYPI_TOR_ID "/status", "up");
      // ... and resubscribe
      mqttClient.subscribe(MYPI_TOR_ID "/relais");
      mqttClient.subscribe(MYPI_TOR_ID "/debug/#");
      mqttClient.subscribe(MYPI_TOR_ID "/open");
      mqttClient.subscribe(MYPI_TOR_ID "/close");
    } else {
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void MqttEnsureConnected() {
  if (!mqttLoopConnected) {
    if (!mqttClient.loop()) {
      mqttReconnect();
    }
    mqttLoopConnected = true;
  }
}

boolean MqttPublish(const char* topic, const char* payload) {
  MqttEnsureConnected();
  return mqttClient.publish(topic, payload);
}

void mqttKeepAlive() {
  static unsigned long lastTime = 0;
  unsigned long now = millis();
  if (!mqttLoopConnected) {
    if ((now-lastTime) < 1000) {
      return;
    }
    MqttEnsureConnected();
  }

  lastTime = millis();
}
