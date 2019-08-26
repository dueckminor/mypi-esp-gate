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
    lastInput = input;

    GatePosition positionCandidate = GatePosition(((~input) >> 1) & 7);
    bool bOpenDirection = ! ((input >> 0) & 1);
    bool bCloseDirection = ! ((input >> 4) & 1);

    if (bOpenDirection && bCloseDirection) {
        // both direction bits are set -> we can be sure that all position
        // bits are correct.
      position = positionCandidate;
    }

    if (DebugEnabled())
    {
        String msg = "00000";
        msg +=String((unsigned long)input, BIN);
        msg = msg.substring(msg.length()-5);
        if (bOpenDirection && bCloseDirection) {
            msg += " (" + String(position,DEC) + ")";
        }
        DebugDump("sensor_bits",msg.c_str());
    }

    GateAnalyseInput(position,bOpenDirection,bCloseDirection);
}

static bool s_bHaveSimulatorInput = false;
static char s_SimulatorInput = 0;

void SensorSimulatorInput(char input)
{
    input &= 0x1f;
    s_SimulatorInput = input;
    s_bHaveSimulatorInput = true;
}


void SensorLoopHandler(char input)
{
    if (s_bHaveSimulatorInput) 
    {
        sensorAnalyseBits(s_SimulatorInput);
        s_bHaveSimulatorInput = false;
    }

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
