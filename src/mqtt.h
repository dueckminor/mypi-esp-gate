#ifndef __MYPI_ESP_DOOR_MQTT_H__
#define __MYPI_ESP_DOOR_MQTT_H__

extern void MqttInitialize();
extern boolean MqttPublish(const char* topic, const char* payload);
extern void MqttBeginLoop();
extern void MqttEnsureConnected();

#endif/*__MYPI_ESP_DOOR_MQTT_H__*/
