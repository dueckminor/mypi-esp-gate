#include "Arduino.h"
#include "debug.h"
#include "mqtt.h"
#include "settings.h"
#include "wifi.h"

void DebugDump(const char * topic, const char * msg)
{
    if (DebugEnabled()) 
    {
        if (WifiIsConnected())
        {
            MqttTopic("debug",topic).Publish(msg);
        }
        Serial.printf("%s: %s\n",topic,msg);
    }
}

bool DebugEnabled()
{
    return true;
}