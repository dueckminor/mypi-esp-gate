#include "Arduino.h"
#include "debug.h"
#include "mqtt.h"
#include "settings.h"

void DebugDump(const char * msg)
{
    MqttPublish(MYPI_TOR_ID "/debug", msg);
}

bool DebugEnabled()
{
    return true;
}