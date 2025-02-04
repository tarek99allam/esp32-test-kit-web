#include "GoogleSheetClient.h"

// Constructor that accepts the Google Sheets URL
GoogleSheetClient::GoogleSheetClient(const char *scriptUrl) : _scriptUrl(scriptUrl)
{
}

// Separate initialization method (called in setup after Wi-Fi connects)
void GoogleSheetClient::begin()
{
    // Any additional setup that needs to happen after Wi-Fi is ready can go here
}


String GoogleSheetClient::createJSONPayload(const TestRecord &record, const finalResult_t &finalResult)
{
    String jsonData = "{";
    jsonData += "\"Device_ID\":\"" + record.uniqueName + "\",";
    jsonData += "\"State\":\"" + finalResult.finalResult + "\",";
    jsonData += "\"dc1_volt_1\":\"" + finalResult.rele1_toggle_signal + "\",";
    jsonData += "\"dc1_volt_2\":\"" + finalResult.rele1_toggle_signal + "\",";
    jsonData += "\"dc2_volt_1\":\"" + finalResult.rele2_toggle_signal + "\",";
    jsonData += "\"dc2_volt_2\":\"" + finalResult.rele2_toggle_signal + "\",";
    jsonData += "\"MQTT1_volt_low\":\"" + finalResult.rele1_toggle_mqtt+ "\",";
    jsonData += "\"MQTT1_volt_high\":\"" + finalResult.rele1_toggle_mqtt + "\",";
    jsonData += "\"MQTT2_volt_low\":\"" + finalResult.rele2_toggle_mqtt + "\",";
    jsonData += "\"MQTT2_volt_high\":\"" + finalResult.rele2_toggle_mqtt + "\",";
    jsonData += "\"Date\":\"" + record.testTime + "\",";
    jsonData += "\"Time\":\"" + record.testTime + "\"";
    jsonData += "}";
    return jsonData;
}

// Method to send data to the Google Sheet
bool GoogleSheetClient::sendData(const String &jsonPayload)
{
    if (!WiFi.isConnected())
    {
        Serial.println("Wi-Fi not connected, cannot send data.");
        return false;
    }

    HTTPClient http;
    http.begin(_scriptUrl);
    http.addHeader("Content-Type", "application/json");

    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0)
    {
        Serial.printf("Data sent to Google Sheets, Response code: %d\n", httpResponseCode);

        http.end(); // Free resources
        return true;
    }
    else
    {
        Serial.printf("Error sending data, HTTP response code: %d\n", httpResponseCode);
    }
    http.end(); // Free resources
    return false;
}
