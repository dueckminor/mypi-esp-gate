#ifndef __MYPI_ESP_GATE_GATE_H__
#define __MYPI_ESP_GATE_GATE_H__

enum GatePosition {
    GatePosition_Undefined = -1,
    GatePosition_0 = 0,
    GatePosition_1 = 1,
    GatePosition_2 = 2,
    GatePosition_3 = 3,
    GatePosition_4 = 4,
    GatePosition_5 = 5,
    GatePosition_6 = 6,
    GatePosition_7 = 7,
    GatePosition_Open = GatePosition_0,
    GatePosition_Closed = GatePosition_7
};

extern void GateLoopHandler();

extern void GateAnalyseInput(
    GatePosition position,  // the last known position
    bool bOpenDirection,    // true if we are over the Sensor, but a little bit in close direction
    bool bCloseDirection    // true if we are over the Sensor, but a little bit in open direction
);

extern void GateRequestPosition(
    GatePosition position
);

#endif/*__MYPI_ESP_GATE_SENSOR_H__*/