#include <EEPROM.h>
#include "sensor.h"
#include "gate.h"
#include "settings.h"
#include "hardware.h"
#include "actor.h"
#include "wifi.h"
#include "mqtt.h"
#include "debug.h"

void setup() {
  HardwareInitialize();
  WifiInitialize();
  MqttInitialize();
}

int loopcount = 0;

void loop() {
  char bits = HardwareRead();
  MqttBeginLoop();

  // Toggle the hardware LED (the blue on an ESP-01) every 1000 loops
  // a constantly flashing LED indicates a good network connection
  // (the faster the better)
  if (0==(loopcount%1000))
  {
    //DebugDump("loopcount",String(loopcount,DEC).c_str());
    //DebugDump("sensor_bits",String((unsigned long)bits,BIN).c_str());
    HardwareLED((loopcount/1000)&1);
  }

  if (0==(loopcount%50))
  {
    MqttEnsureConnected();
  }
  ActorLoopHandler();
  SensorLoopHandler(bits);

  GateLoopHandler();

  loopcount++;
}
