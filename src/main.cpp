#include <EEPROM.h>
#include "sensor.h"
#include "gate.h"
#include "settings.h"
#include "hardware.h"
#include "actor.h"
#include "wifi.h"
#include "mqtt.h"
#include "debug.h"
#include "timer.h"

void setup()
{
  Serial.begin(74880);

  HardwareInitialize();
  WifiInitialize();
  MqttInitialize();
  DebugDump("loopcount", "0 (restart)");
}

int loopcount = 0;

void loop()
{
  TimerLoop();
  WifiLoop();
  HardwareLoop();

  unsigned char bits = HardwareHaveEvents();
  MqttBeginLoop();

  MqttEnsureConnected();
  ActorLoopHandler();

  for (;HardwareHaveEvents();)
  {
    bits = HardwareRead();

    DebugDump("sensor_bits",String((unsigned long)bits,BIN).c_str());

    SensorLoopHandler(bits);
  }

  GateLoopHandler();

  loopcount++;
}
