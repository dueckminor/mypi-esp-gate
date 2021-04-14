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
  TimerSetBlinkPatternInit();

  HardwareInitialize();
  WifiInitialize();

  int cWPS = 0;
  while (!WifiSetupLoop()) {
    TimerLoop();
    delay(100);
    if (HardwareReadWPS()) {
      cWPS++;
    } else {
      cWPS = 0;
    }

    if (cWPS == 30) {
      HardwareLED(true);
      WifiInitializeWPS();
      cWPS = 0;
    }
  }

  MqttInitialize();
  DebugDump("loopcount", "0 (restart)");
}

int loopcount = 0;

void loop()
{
  TimerLoop();
  HardwareLoop();

  if (!WifiLoop()) {
    TimerSetBlinkPatternSOS();
    delay(100);
    return;
  }

  TimerSetBlinkPatternOK();

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
  DebugLoop();

  loopcount++;
}
