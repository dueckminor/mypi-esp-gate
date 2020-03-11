#include "Arduino.h"
#include "debug.h"
#include "mqtt.h"
#include "settings.h"
#include "wifi.h"

#define MAX_LINES 64
static String aszLines[MAX_LINES];
static String aszTopic[MAX_LINES];
static int cLines=0;
static uint8_t iLineRead=0;
static uint8_t iLineWrite=0;

static void debugAddLine(const char * topic, const char * msg) 
{
    if (cLines==MAX_LINES) {
        iLineRead++;
        iLineRead %= MAX_LINES;
        cLines--;
    }

    aszTopic[iLineWrite] = topic;
    aszLines[iLineWrite] = msg;

    iLineWrite++;
    iLineWrite %= MAX_LINES;
    cLines++;

    if (cLines > (MAX_LINES/2)) 
    {
        DebugLoop();
    }
}

void DebugDump(const char * topic, const char * msg)
{
    if (!msg)
    {
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
            if (szLine[cchLine-1]!='\n') {
                szLine[cchLine]='\n';
                szLine[cchLine+1]='\0';
            }
            debugAddLine(topic,szLine);
        }
    }
}

void DebugLoop() {
    for (;cLines;)
    {
        if (!WifiIsConnected())
        {
            break;
        }
        MqttTopic("debug",aszTopic[iLineRead].c_str()).Publish(aszLines[iLineRead].c_str());
        aszTopic[iLineRead].clear();
        aszLines[iLineRead].clear();
        iLineRead++;
        iLineRead %= MAX_LINES;
        cLines--;
    }
}

bool DebugEnabled()
{
    return true;
}