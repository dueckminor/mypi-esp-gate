#ifndef __MYPI_ESP_GATE_MQTT_H__
#define __MYPI_ESP_GATE_MQTT_H__

extern void MqttInitialize();
extern bool MqttPublish(const char* topic, const char* payload);
extern void MqttBeginLoop();
extern void MqttEnsureConnected();

class MqttTopic {
protected:
    String m_topic;
public:
    MqttTopic(const char * topic);
    MqttTopic(const char * group, const char * topic);

    bool Publish(const char * payload);
};

#endif/*__MYPI_ESP_GATE_MQTT_H__*/
