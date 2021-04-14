#include <Arduino.h>
#include "gate.h"
#include "settings.h"
#include "mqtt.h"
#include "actor.h"

static GatePosition g_position = GatePosition_Undefined;

static GatePosition g_positionRequested = GatePosition_Undefined;
static GatePosition g_positionStart = GatePosition_Undefined;
static GatePosition g_positionExpectedNext = GatePosition_Undefined;
static unsigned long g_timePositionRequestStart = 0;
static unsigned long g_timePositionLastChange = 0;
static unsigned long g_timeBeginWait = 0;
static unsigned long g_timeWaitFor = 0;

static MqttTopic mqttTopicPosition("position");

static void gateReportPosition() {
    static GatePosition positionReported = GatePosition_Undefined;
    static unsigned long lastReportedMillis = 0;
    unsigned long now = millis();
    if (positionReported == g_position) {
        if ((now - lastReportedMillis) < 60*1000) {
            // publish the position every 60 seconds
            return;
        }
    }
    mqttTopicPosition.Publish(String(g_position, DEC).c_str());

    positionReported = g_position;
    lastReportedMillis = now;
}

void GateAnalyseInput(
    GatePosition position, // the last known position
    bool bOpenDirection, // true if we are over the Sensor, but a little bit in close direction
    bool bCloseDirection // true if we are over the Sensor, but a little bit in open direction
)
{
    static bool s_bLastOpenDirection = false;
    static bool s_bLastCloseDirection = false;

    //bool bOverSensor = bOpenDirection || bCloseDirection;
    bool bExactlyOverSensor = bOpenDirection && bCloseDirection;

    if (bExactlyOverSensor)
    {
        g_position = position;
        gateReportPosition();
    }
    else if (bOpenDirection || bCloseDirection)
    {
        if (s_bLastOpenDirection && s_bLastCloseDirection)
        {
            if (bOpenDirection) {
                if (g_position > GatePosition_Open)
                {
                    g_positionExpectedNext = (GatePosition)(g_position-1);
                }
            }
            else
            {
                if (g_position < GatePosition_Closed)
                {
                    g_positionExpectedNext = (GatePosition)(g_position+1);
                }
            }
        }
    }

    s_bLastOpenDirection = bOpenDirection;
    s_bLastCloseDirection = bCloseDirection;

    g_timePositionLastChange = millis();
}

void GateRequestPosition(
    GatePosition position
)
{
    if (position == g_position) {
        return;
    }

    g_positionRequested = position;
    g_positionStart = g_position;
    g_timePositionRequestStart = millis();
    if (!g_timePositionRequestStart) {
        g_timePositionRequestStart = 1;
    }

    ActorRelaisOn();
}

void GateLoopHandler()
{
    gateReportPosition();

    unsigned long now = millis();
    if (now-g_timePositionLastChange > 1000*30)
    {
        // no events since 30 seconds -> expect gate has stopped.
        g_positionExpectedNext = GatePosition_Undefined;
    }

    if (g_timeWaitFor && (now-g_timeBeginWait > g_timeWaitFor)) {
        // we should wait for some time, but this time is over.
        g_timeWaitFor = 0;
    }
}