// config.h
#ifndef DEFINES_H
#define DEFINES_H

#include <Arduino.h>
#include <string>

struct TestRecord
{
    String uniqueName;
    String finalResult;
    String line1_volt1;
    String line1_volt2;
    String line2_volt1;
    String line2_volt2;
    String line1_mqtt_l;
    String line1_mqtt_h;
    String line2_mqtt_l;
    String line2_mqtt_h;
    String testTime;
};
struct finalResult_t
{
    String uniqueName;
    String finalResult;
    String googlesheet_State;
    String rele1_toggle_mqtt;
    String rele2_toggle_mqtt;
    String rele2_toggle_signal;
    String rele1_toggle_signal;
};
struct checkbox
{
    String buzzer;
    String indicator;
    String reset;
};
enum class final_state
{
    PASSED,
    FAILED
};

void clearTestRecord(TestRecord &record);
void clearCheckbox(checkbox &cb);
typedef void (*MQTTMessageCallback)(const char *topic, const char *message);

String generateTestResultHTML(const TestRecord &record);
// WiFi credentials
#define WIFI_SSID "ADD_YOURS" // Replace with your WiFi SSID
#define WIFI_PASSWORD "ADD_YOURS" // Replace with your WiFi password

#define MQTT_BROKER_URI "ADD_YOURS"
#define MQTT_PORT ADD_YOURS 
#define MQTT_USERNAME "ADD_YOURS" // Optional, use "" if not needed
#define MQTT_PASSWORD "ADD_YOURS" // Optional, use "" if not needed

// MQTT Topics
#define MQTT_TOPIC_ACK "kit_iwstest1/ack"
#define MQTT_TOPIC_PUBLISH "kit_iwstest1/starttest"
#define MQTT_TOPIC_SUBSCRIBE "kit_iwstest1/target_name_001"

// #define MQTT_TOPIC_RECEIVER "ir_testkit1/receiver"
// #define MQTT_TOPIC_TRANSMIT "ir_testkit1/command"

#define MAX_ARRAY_SIZE 256 // Define the maximum size of the global array

#define BUTTON1_PIN D23
#define BUTTON2_PIN D25
#define LED_START_PIN D26
#define LED_READY_PIN D27
#define LED_PROGRESS_PIN D32
#define LED_PASSED_PIN D33
#define LED1_FAiled_PIN D13

// NTP server and time configuration
#define NTP_SERVER "pool.ntp.org"
#define GMT_OFFSET_SEC (2 * 3600) // UTC+2 for Egypt
#define DAYLIGHT_OFFSET_SEC 0 // No daylight saving time

// mDNS hostname
#define MDNS_HOSTNAME "testkit-iws"
// Define the pin for the interrupt
#define INTERRUPT_PIN 35
#define WEBSOCKET_SERVER 81

#define GOOGLE_SCRIPT_URL "ADD_YOURS" 
#define WIFI_ERROR_NUMBER 0
#define IR_RECEIVER_ERROR_NUMBER 1
#define IR_TRANSIMET_ERROR_NUMBER 2
#define INDICATOR_ERROR_NUMBER 3
#define BUZZER_ERROR_NUMBER 4
#define ANNTENA_ERROR_NUMBER 5
#define TIME_ERROR_NUMBER 6
#define SAVE_ERROR_NUMBER 7
#endif
