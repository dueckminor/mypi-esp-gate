#include <Arduino.h>
#include "gate.h"
#include "settings.h"
#include "mqtt.h"

extern void DoorAnalyseInput(
    DoorPosition position,     // the last known position
    bool bOpenDirection, // true if we are over the Sensor, but a little bit in close direction
    bool bCloseDirection // true if we are over the Sensor, but a little bit in open direction
)
{
    //bool bOverSensor = bOpenDirection || bCloseDirection;
    bool bExactlyOverSensor = bOpenDirection && bCloseDirection;

    static DoorPosition positionReported = DoorPosition_Undefined;
    if (bExactlyOverSensor && position != positionReported)
    {
        String msg = String(position, DEC);
        MqttPublish(MYPI_TOR_ID "/position", msg.c_str());
        msg = String(millis(), DEC);
        MqttPublish(MYPI_TOR_ID "/time", msg.c_str());
        positionReported = position;
    }
}

extern void DoorRequestPosition(
    DoorPosition position
)
{

}
