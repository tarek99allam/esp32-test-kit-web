#include "MQTTManager.h"
#include "..\defines.h"

MQTTManager::MQTTManager(Client &networkClient, const char *server, uint16_t port) : client(networkClient)
{
    client.setServer(server, port);
    username = nullptr;
    password = nullptr;
    jsonCallback = nullptr;
}

void MQTTManager::setCredentials(const char *user, const char *pass)
{
    username = user;
    password = pass;
}

void MQTTManager::setCallback(MQTT_CALLBACK_SIGNATURE)
{
    client.setCallback(callback);
}

bool MQTTManager::connect(const char *clientId)
{
    if (username && password)
    {
        return client.connect(clientId, username, password);
    }
    else
    {
        return client.connect(clientId);
    }
}

void MQTTManager::loop()
{
    client.loop();
}

bool MQTTManager::publish(const char *topic, const char *payload)
{
    return client.publish(topic, payload);
}

bool MQTTManager::publishJSON(const char *topic, cJSON *jsonPayload)
{
    Serial.printf("\n\nsenttt");
    if (!jsonPayload)
    {
        return false;
    }

    char *jsonString = cJSON_PrintUnformatted(jsonPayload);
    bool result = client.publish(topic, jsonString);
    free(jsonString); // Free the memory allocated by cJSON_PrintUnformatted
    return result;
}

bool MQTTManager::subscribe(const char *topic, uint8_t qos)
{
    return client.subscribe(topic, qos);
}

bool MQTTManager::subscribeJSON(const char *topic, uint8_t qos, void (*callback)(const char *, cJSON *))
{
    if (!topic || !callback)
    {
        return false;
    }

    jsonCallback = callback; // Set the JSON callback

    // Set an internal callback to process messages and extract JSON
    client.setCallback(
        [this](char *topic, byte *payload, unsigned int length) { this->handleJSONMessage(topic, payload, length); });

    return client.subscribe(topic, qos);
}

void MQTTManager::handleJSONMessage(char *topic, byte *payload, unsigned int length)
{
    if (!jsonCallback)
    {
        return;
    }

    // Convert payload to a null-terminated string
    char message[length + 1];
    strncpy(message, (char *)payload, length);
    message[length] = '\0';

    // Parse JSON
    cJSON *json = cJSON_Parse(message);
    if (!json)
    {
        Serial.println("Failed to parse JSON message.");
        return;
    }

    // Invoke the user-defined callback with the topic and parsed JSON
    jsonCallback(topic, json);

    // Clean up
    cJSON_Delete(json);
}
