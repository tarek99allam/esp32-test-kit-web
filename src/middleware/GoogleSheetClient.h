#ifndef GOOGLE_SHEET_CLIENT_H
#define GOOGLE_SHEET_CLIENT_H

#include "../defines.h"
#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFi.h>

class GoogleSheetClient
{
  public:
    // Constructor that accepts the Google Sheets URL
    GoogleSheetClient(const char *scriptUrl);

    // Initialization method to ensure all resources are ready
    void begin();

    // Method to create a JSON payload with the new fields
    String createJSONPayload(const TestRecord &record, const finalResult_t &finalResult);

    // Method to send data to the Google Sheet
    bool sendData(const String &jsonPayload);

  private:
    const char *_scriptUrl; // URL of the Google Script Web App (const to optimize memory)
};

#endif // GOOGLE_SHEET_CLIENT_H
