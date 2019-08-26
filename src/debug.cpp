#include "Arduino.h"
#include "debug.h"
#include "mqtt.h"
#include "settings.h"

void DebugDump(const char * topic, const char * msg)
{
    if (DebugEnabled()) 
    {
        MqttTopic("debug",topic).Publish(msg);
    }
}

bool DebugEnabled()
{
    return true;
}