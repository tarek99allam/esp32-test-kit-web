#include "WiFiManager.h"

// Constructor
WiFiManager::WiFiManager(const char *ssid, const char *password) : _ssid(ssid), _password(password)
{
}

// Connect to Wi-Fi
void WiFiManager::connect()
{
    WiFi.begin(_ssid, _password);
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nConnected to WiFi");

    // Show the IP address
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
}

// Get the local IP address as a String
String WiFiManager::getIPAddress()
{
    return WiFi.localIP().toString();
}
