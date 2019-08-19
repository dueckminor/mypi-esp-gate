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

  String msg = String((int)(sizeof(unsigned long)), DEC);
  MqttPublish(MYPI_TOR_ID "/debug/sizeof_unsigned_long", msg.c_str());

}

int loopcount = 0;
String dbgMsg = "";

static void MqttSendStatistics() {
    static uint64_t lastTime = 0;
    uint64_t thisTime = micros64();

    if ((thisTime - lastTime) < 5000000L) {
      return;
    }

    String msg = String((int)(thisTime-lastTime), DEC);
    MqttPublish(MYPI_TOR_ID "/loop/time", msg.c_str());
    if (dbgMsg != "") {
      MqttPublish(MYPI_TOR_ID "/debug", dbgMsg.c_str());
      dbgMsg = "";
    }
    lastTime = thisTime;
}

void loop() {
  // Toggle the hardware LED (the blue on an ESP-01) every 1000 loops
  // a constantly flashing LED indicates a good network connection
  // (the faster the better)
  if (0==(loopcount%1000)) {
    HardwareLED((loopcount/1000)&1);
  }
  loopcount++;

  MqttBeginLoop();

  ActorLoop();
  SensorAnalyseInput(HardwareRead());

  GateLoopHandler();

  MqttEnsureConnected();
  MqttSendStatistics();
}