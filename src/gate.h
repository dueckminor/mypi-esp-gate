#ifndef __MYPI_ESP_DOOR_DOOR_H__
#define __MYPI_ESP_DOOR_DOOR_H__

enum DoorPosition {
    DoorPosition_Undefined = -1,
    DoorPosition_0 = 0,
    DoorPosition_1 = 1,
    DoorPosition_2 = 2,
    DoorPosition_3 = 3,
    DoorPosition_4 = 4,
    DoorPosition_5 = 5,
    DoorPosition_6 = 6,
    DoorPosition_7 = 7,
    DoorPosition_Open = DoorPosition_0,
    DoorPosition_Closed = DoorPosition_7
};

extern void DoorAnalyseInput(
    DoorPosition position,  // the last known position
    bool bOpenDirection,    // true if we are over the Sensor, but a little bit in close direction
    bool bCloseDirection    // true if we are over the Sensor, but a little bit in open direction
);

extern void DoorRequestPosition(
    DoorPosition position
);

#endif/*__MYPI_ESP_DOOR_SENSOR_H__*/