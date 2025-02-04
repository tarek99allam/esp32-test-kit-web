#ifndef WIFIMANAGER_H
#define WIFIMANAGER_H

#include <WiFi.h>

class WiFiManager
{
  public:
    WiFiManager(const char *ssid, const char *password);

    // Connect to Wi-Fi
    void connect();

    // Get the local IP address
    String getIPAddress();

  private:
    const char *_ssid;
    const char *_password;
};

#endif // WIFIMANAGER_H
