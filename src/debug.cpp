#include "Arduino.h"
#include "debug.h"
#include "mqtt.h"
#include "settings.h"
#include "wifi.h"

static char aszLines[32][80];
static char aszTopic[32][16];
static int cLines=0;
static uint8_t iLineRead=0;
static uint8_t iLineWrite=0;

static void debugAddLine(const char * topic, const char * msg) 
{
    if (cLines==32) {
        iLineRead++;
        iLineRead %= 32;
        cLines--;
    }

    strncpy(aszTopic[iLineWrite],topic,15);
    aszTopic[iLineWrite][15]='\0';

    strncpy(aszLines[iLineWrite],msg,79);
    aszLines[iLineWrite][79]='\0';

    iLineWrite++;
    iLineWrite %= 32;
    cLines++;
}

void DebugDump(const char * topic, const char * msg)
{
    Serial.printf("DebugDump()\n");
    if (!msg)
    {
        Serial.printf("DebugDump() done\n");
        return;
    }
    if (DebugEnabled()) 
    {
        Serial.printf("%s: %s\n",topic,msg);
        char szLine[81];
        int cchLine=0;

        for (;*msg;)
        {
            char ch = *msg++;
            szLine[cchLine++]=ch;
            szLine[cchLine]='\0';
            if ((ch == '\n') || cchLine==80)
            {
                debugAddLine(topic,szLine);
                cchLine = 0;
            }
        }
        if (cchLine) {
            debugAddLine(topic,szLine);
        }
    }
    Serial.printf("DebugDump() done\n");
}

void DebugLoop() {
    for (;cLines;)
    {
        if (!WifiIsConnected())
        {
            break;
        }
        MqttTopic("debug",aszTopic[iLineRead]).Publish(aszLines[iLineRead]);
        iLineRead++;
        iLineRead %= 32;
        cLines--;
    }
}

bool DebugEnabled()
{
    return true;
}