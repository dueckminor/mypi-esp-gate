#include <EEPROM.h>
#include "sensor.h"
#include "gate.h"
#include "settings.h"
#include "hardware.h"
#include "actor.h"
#include "wifi.h"
#include "mqtt.h"


void setup() {
  HardwareInitialize();
  WifiInitialize();
  MqttInitialize();
}

int loopcount = 0;
String dbgMsg = "";

static void MqttSendStatistics() {
    static uint64_t lastTime = 0;
    uint64_t thisTime = micros64();

    if ((thisTime - lastTime) < 5000000L) {
      return;
    }

    String msg = String(loopcount, DEC);
    MqttPublish(MYPI_TOR_ID "/loop/count", msg.c_str());
    msg = String((int)(thisTime-lastTime), DEC);
    MqttPublish(MYPI_TOR_ID "/loop/time", msg.c_str());
    if (dbgMsg != "") {
      MqttPublish(MYPI_TOR_ID "/debug", dbgMsg.c_str());
      dbgMsg = "";
    }
    lastTime = thisTime;
}

void loop() {
  MqttBeginLoop();

  ActorLoop();
  SensorAnalyseInput(HardwareRead());

  if (0==(loopcount%1000)) {
    HardwareLED((loopcount/1000)&1);
  }

  MqttEnsureConnected();
  MqttSendStatistics();

  loopcount++;
}