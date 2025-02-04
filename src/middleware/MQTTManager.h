#ifndef MQTTDRIVER_H
#define MQTTDRIVER_H

#include <PubSubClient.h>
#include <WiFi.h>
#include <cJSON.h>


class MQTTManager
{
  public:
    MQTTManager(Client &networkClient, const char *server, uint16_t port);

    void setCredentials(const char *username, const char *password);
    void setCallback(MQTT_CALLBACK_SIGNATURE);

    bool connect(const char *clientId);
    void loop();
    bool publish(const char *topic, const char *payload);
    bool publishJSON(const char *topic, cJSON *jsonPayload);
    bool subscribe(const char *topic, uint8_t qos = 0);
    bool subscribeJSON(const char *topic, uint8_t qos, void (*callback)(const char *, cJSON *));

  private:
    PubSubClient client;
    const char *username;
    const char *password;
    void (*jsonCallback)(const char *, cJSON *); // JSON callback

    // Internal handler for JSON messages
    void handleJSONMessage(char *topic, byte *payload, unsigned int length);
};

#endif
