#include <Arduino.h>
#include "sensor.h"
#include "gate.h"
#include "debug.h"

void sensorAnalyseBits(char input)
{
    static char lastInput=0xff;
    static GatePosition position = GatePosition_Undefined;

    if (input == lastInput)
    {
        // input hasn't changed -> nothing todo
        return;
    }

    GatePosition positionCandidate = GatePosition(((~input) >> 1) & 7);
    bool bOpenDirection = ! ((input >> 0) & 1);
    bool bCloseDirection = ! ((input >> 4) & 1);

    if (bOpenDirection && bCloseDirection) {
        // both direction bits are set -> we can be sure that all position
        // bits are correct.
      position = positionCandidate;
    }

    GateAnalyseInput(position,bOpenDirection,bCloseDirection);
}

void SensorAnalyseInput(char input)
{
     // mask out all irelevant sensor bits
    input &= 0x1f;

    static char lastInput = 0xff;
    if (lastInput == input)
    {
        return;
    }
    lastInput = input;
    sensorAnalyseBits(input);
}

void SensorAnalyseDebugInput(char input)
{
    // mask out all irelevant sensor bits
    input &= 0x1f;
    sensorAnalyseBits(input);
}
