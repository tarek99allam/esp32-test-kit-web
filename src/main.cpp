#include "cJSON.h"
#include "defines.h"
#include "middleware/GPIO.h"
#include "middleware/GoogleSheetClient.h"
#include "middleware/InterruptHandler.h"
#include "middleware/MDNSManager.h"
#include "middleware/MQTTManager.h"
#include "middleware/TimeSyncManager.h"
#include "middleware/WebServerManager.h"
#include "middleware/WiFiManager.h"
#include <ESPAsyncWebServer.h>
#include <arduino-timer.h>
#include <vector>

// Forward declarations
void handleStartTest();
bool checkTestCompelet(void *opaque);
void onJSONMessageReceived(const char *topic, cJSON *json);
void sendStartSignal();
int receivedArr[MAX_ARRAY_SIZE];
int receivedArrLength = 0; // To track the length of the array

// Global Instances
AsyncWebServer server(80);
WiFiManager wifiManager(WIFI_SSID, WIFI_PASSWORD);
MDNSManager mdnsManager(MDNS_HOSTNAME);
TimeSyncManager timeSyncManager(NTP_SERVER, GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC);
InterruptHandler interruptHandler(INTERRUPT_PIN);
GoogleSheetClient *googleSheetClient = nullptr;

WiFiClient espClient;                                           // Fix: Declare WiFiClient as a persistent variable
MQTTManager mqttManager(espClient, MQTT_BROKER_URI, MQTT_PORT); // Pass by reference

WebServerManager webServerManager(server); // Pass by reference
/*********************** GLOBAL VARIABLES *************************** */
String targetName;
String target_rele1;
String target_rele2;

// Timer
auto timer = timer_create_default();
struct rele_states
{
    uint8_t rele1;
    uint8_t rele2;
};
struct rele_states releStates[4];
// State Variables
bool isTestRunning = false;
bool isTestDataReady = false;
bool got_target_name_flag = false;
int result_page = 0;
int testRunning = 0;
int timeout_mqtt = 0;
String savedInSheet = "";
uint16_t testCollect = 0;

checkbox checkbox_result;
TestRecord testRecord;
finalResult_t finalResult;
// Helper Functions
void printHeapUsage()
{
    Serial.print("[INFO] Free heap memory: ");
    Serial.print(esp_get_free_heap_size());
    Serial.println(" bytes");
}

bool runTest(void *arg)
{
    switch (testRunning)
    {
    case 0:
        // testRecord.line1_volt1 = digitalRead(OUTPUT_RELE1) ? "220V" : "0V";
        // testRecord.line2_volt1 = digitalRead(OUTPUT_RELE2) ? "220V" : "0V";
        releStates[0].rele1 = digitalRead(OUTPUT_RELE1);
        releStates[0].rele2 = digitalRead(OUTPUT_RELE2);
        testRunning++;
        break;
    case 1:
        digitalWrite(DC_TOGGLE_LINE1, !digitalRead(DC_TOGGLE_LINE1));
        testRunning++;
        break;
    case 2:
        digitalWrite(DC_TOGGLE_LINE2, !digitalRead(DC_TOGGLE_LINE2));
        testRunning++;
        break;
    case 3:
        releStates[1].rele1 = digitalRead(OUTPUT_RELE1);
        releStates[1].rele2 = digitalRead(OUTPUT_RELE2);
        testRecord.line1_volt2 = digitalRead(OUTPUT_RELE1) ? "220V" : "0V";
        testRecord.line2_volt2 = digitalRead(OUTPUT_RELE2) ? "220V" : "0V";

        testRunning++;
        timeout_mqtt = 0;
        break;
    case 4:
        // mqtt
        if (got_target_name_flag)
        {
            result_page = 3;
            mqttManager.publish(target_rele1.c_str(), "ON");
            testRunning++;
        }
        else
        {
            timeout_mqtt++;
            if (timeout_mqtt > 50)
            {
                Serial.printf("[timeout] mqtt time out \n");
                result_page = MQTT_TIMEOUT;
                return false;
            }
            if (timeout_mqtt % 5)
            {
                mqttManager.publish(MQTT_TOPIC_PUBLISH, "{\"test\":\"start\"}");
            }
            result_page = 2;
        }
        break;
    case 5:
        // mqtt
        mqttManager.publish(target_rele2.c_str(), "ON");
        testRunning++;

        break;
    case 6:
        // mqtt
        releStates[2].rele1 = digitalRead(OUTPUT_RELE1);
        releStates[2].rele2 = digitalRead(OUTPUT_RELE2);

        testRecord.line1_mqtt_l = digitalRead(OUTPUT_RELE1) ? "220V" : "0V";
        testRecord.line2_mqtt_l = digitalRead(OUTPUT_RELE2) ? "220V" : "0V";
        testRunning++;

        break;
    case 7:
        // mqtt
        mqttManager.publish(target_rele1.c_str(), "OFF");
        testRunning++;

        break;
    case 8:
        mqttManager.publish(target_rele2.c_str(), "OFF");
        testRunning++;
        break;
    case 9:
        releStates[3].rele1 = digitalRead(OUTPUT_RELE1);
        releStates[3].rele2 = digitalRead(OUTPUT_RELE2);
        // mqtt
        testRecord.line1_mqtt_h = digitalRead(OUTPUT_RELE1) ? "220V" : "0V";
        testRecord.line2_mqtt_h = digitalRead(OUTPUT_RELE2) ? "220V" : "0V";
        testRunning++;
        // printTestRecord();

        break;
    case 10:
        result_page = 5;

        return false;

        break;
    }
    Serial.printf("[testRunning] %d \n", testRunning);
    // printTestRecord();

    return true;
}
void handleStartTest()
{
    result_page = 0;
    timeout_mqtt = 0;
    got_target_name_flag = false;
    testRunning = 0;
    finalResult.finalResult = "";
    testRecord.testTime = timeSyncManager.getCurrentTime();
    timer.every(200, checkTestCompelet, NULL);
    timer.every(500, runTest, NULL);

    Serial.println("[Info] Starting test from WebServer.");
    sendStartSignal();
    result_page = 1;
}

void check_finalresult()
{

    // if (testRecord.line1_volt1 != testRecord.line1_volt2)
    if (releStates[0].rele1 != releStates[1].rele1)
    {
        finalResult.rele1_toggle_signal = "PASSED";
    }
    else
    {
        finalResult.rele1_toggle_signal = "FAILED";
    }

    if (releStates[0].rele2 != releStates[1].rele2)
    // if (testRecord.line2_volt1 != testRecord.line2_volt2)
    {
        finalResult.rele2_toggle_signal = "PASSED";
    }
    else
    {
        finalResult.rele2_toggle_signal = "FAILED";
    }
    /*************************DONE  */
    if (releStates[2].rele1 != releStates[3].rele1)
    {
        finalResult.rele1_toggle_mqtt = "PASSED";
    }
    else
    {
        finalResult.rele1_toggle_mqtt = "FAILED";
    }

    if (releStates[2].rele2 != releStates[3].rele2)
    // if (testRecord.line2_mqtt_l != testRecord.line2_mqtt_h)
    {
        finalResult.rele2_toggle_mqtt = "PASSED";
    }
    else
    {
        finalResult.rele2_toggle_mqtt = "FAILED";
    }

    /********************************************************************** */

    if (finalResult.rele1_toggle_signal == "FAILED" && finalResult.rele1_toggle_mqtt == "PASSED")
    {
        finalResult.finalResult = "switch 1 FAILED ";
    }
    if (finalResult.rele1_toggle_mqtt == "FAILED" && finalResult.rele1_toggle_signal == "FAILED")
    {
        finalResult.finalResult = "Rele1 Failed";
    }
    if (finalResult.rele2_toggle_signal == "FAILED" && finalResult.rele2_toggle_mqtt == "PASSED")
    {
        finalResult.finalResult += "switch 2 FAILED";
    }
    if (finalResult.rele2_toggle_mqtt == "FAILED" && finalResult.rele2_toggle_signal == "FAILED")
    {
        finalResult.finalResult += "Rele2 Failed";
    }
    if (finalResult.rele1_toggle_mqtt == "PASSED" && finalResult.rele2_toggle_mqtt == "PASSED" &&
        finalResult.rele1_toggle_signal == "PASSED" && finalResult.rele2_toggle_signal == "PASSED")
    {
        finalResult.finalResult = "ALL PASSED";
    }
}

bool checkTestCompelet(void *arg)
{
    if (result_page <= 4)
    {
    }
    else
    {
        if (result_page == MQTT_TIMEOUT)
        {
            return false;
        }
        else
        {
        }
        check_finalresult();
        testRecord.finalResult = finalResult.finalResult;
        result_page = 5;
        String jsonFile = googleSheetClient->createJSONPayload(testRecord, finalResult);
        bool check_googlesheet = googleSheetClient->sendData(jsonFile);

        if (check_googlesheet)
        {
            finalResult.googlesheet_State = "PASSED";
        }
        else
        {
            finalResult.googlesheet_State = "FAILED";
        }
        result_page = TEST_COMPLETE;

        return false;
    }
    return true;
}

void sendStartSignal()
{
    // Example MQTT signal or any other start signal implementation
    Serial.println("[Info] Sending start signal.");
    mqttManager.publish(MQTT_TOPIC_PUBLISH, "{\"test\":\"start\"}");

    // timer.every(2000, checkTestCompelet,NULL);
}
void onJSONMessageReceived(const char *topic, cJSON *json)
{
    Serial.printf("[Info] JSON message received on topic: %s\n", topic);

    // Extract the "name" field from the JSON object
    cJSON *nameItem = cJSON_GetObjectItem(json, "name");
    if (nameItem && cJSON_IsString(nameItem))
    {
        String targetName = String(nameItem->valuestring); // Safely extract the string value
        Serial.println(targetName);
        testRecord.uniqueName = targetName;
        target_rele1 = targetName + "/switch/switch1_" + targetName.substring(11) + "/command";
        target_rele2 = targetName + "/switch/switch2_" + targetName.substring(11) + "/command";

        mqttManager.publish(MQTT_TOPIC_ACK, "got name");
        got_target_name_flag = true;
    }
    else
    {
        Serial.println("[Error] Name field not found in JSON or invalid format");
    }
}

// Function to handle the received JSON message
void callback_receivedSignal(const char *topic, cJSON *json)
{
    // Validate input parameters
    if (!topic || !json)
    {
        printf("Invalid input to onJSONMessageReceived\n");
        return;
    }

    // Log the topic
    printf("Topic received: %s\n", topic);

    // Parse the JSON content
    cJSON *length = cJSON_GetObjectItem(json, "length");
    cJSON *arr = cJSON_GetObjectItem(json, "arr");

    // Ensure the keys exist and have the correct types
    if (!cJSON_IsNumber(length) || !cJSON_IsArray(arr))
    {
        printf("Invalid JSON structure\n");
        return;
    }

    // Read the "length" value
    int len = length->valueint;
    printf("Length: %d\n", len);

    // Get the array size
    int arrSize = cJSON_GetArraySize(arr);

    // Ensure we don't exceed the maximum array size
    if (arrSize > MAX_ARRAY_SIZE)
    {
        printf("Warning: Array size exceeds maximum limit (%d). Truncating.\n", MAX_ARRAY_SIZE);
        arrSize = MAX_ARRAY_SIZE;
    }

    // Process the "arr" array
    cJSON *element = NULL;
    int index = 0;

    cJSON_ArrayForEach(element, arr)
    {
        if (index >= arrSize)
            break; // Ensure we don't write beyond the allocated size

        if (cJSON_IsNumber(element))
        {
            receivedArr[index++] = element->valueint;
        }
        else
        {
            printf("Unexpected non-numeric element in array\n");
        }
    }

    // Update the global array length
    receivedArrLength = index;

    // Log the global array contents
    printf("Received array values (stored in global array):\n");
    for (int i = 0; i < receivedArrLength; i++)
    {
        printf("[%d]: %d\n", i, receivedArr[i]);
    }

    printf("JSON processing complete.\n");
}
// Setup Functions

void setupWiFi()
{
    wifiManager.connect();
    mdnsManager.start();
}

void setupTimeSync()
{
    timeSyncManager.synchronize();
}

void setupWebServer()
{
    // if (!SPIFFS.begin(true))
    // {
    //     Serial.println("SPIFFS Mount Failed");
    //     return;
    // }
    // Serial.println("SPIFFS mounted successfully");
    webServerManager.configureRoutes();
    webServerManager.onStartTest = handleStartTest; // Attach callback
    server.begin();
    Serial.println("[Info] WebServer started.");
}

void setupGoogleSheetClient()
{
    googleSheetClient = new GoogleSheetClient(GOOGLE_SCRIPT_URL);
    googleSheetClient->begin();
}

void setupMQTT()
{
    mqttManager.setCredentials(MQTT_USERNAME, MQTT_PASSWORD);

    while (!mqttManager.connect("testKIT-IR"))
    {
        Serial.println("[Info] Retrying MQTT connection...");
        delay(2000);
    }

    mqttManager.subscribeJSON(MQTT_TOPIC_SUBSCRIBE, 0, onJSONMessageReceived);
    // mqttManager.subscribeJSON(MQTT_TOPIC_RECEIVER, 0, callback_receivedSignal);
    Serial.println("[Info] MQTT connected and subscribed.");
}

void setupInterruptHandler()
{
    interruptHandler.begin();
    Serial.println("[Info] InterruptHandler initialized.");
}

// Main Setup and Loop
void setup()
{

    Serial.begin(115200);
    printHeapUsage();
    Serial.println("[Info] Starting setup...");
    setupWiFi();
    setupTimeSync();
    printHeapUsage();
    setupWebServer();
    setupGoogleSheetClient();
    setupMQTT();
    setupInterruptHandler();
    printHeapUsage();
    init_gpio();
    Serial.println("[Info] Setup completed.");
}

void loop()
{
    timer.tick();       // Process scheduled tasks
    mqttManager.loop(); // Process MQTT events
}
