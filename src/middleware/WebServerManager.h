#ifndef WEBSERVERMANAGER_H
#define WEBSERVERMANAGER_H

#include "../defines.h"
#include <ESPAsyncWebServer.h>
#include <SPIFFS.h>
#include <functional>
#include <vector>

#define TEST_COMPLETE 100
#define MQTT_TIMEOUT 99


extern int result_page; // Represents the current process state
extern checkbox checkbox_result;
extern finalResult_t finalResult;
extern TestRecord testRecord;
extern bool reset_button_flag;
extern uint16_t testCollect;
extern String targetName;

class WebServerManager
{
  public:
    WebServerManager(AsyncWebServer &server);
    void configureRoutes();
    void setTestResultCallback(std::function<void(AsyncWebServerRequest *request)> callback);
    std::function<void()> onStartTest; // Callback for starting a test
    bool isTestDataReady = false;      // Flag for test data readiness

  private:
    AsyncWebServer &_server;
    // HTMLGenerator &_htmlGenerator;
    // std::vector<TestRecord> &_testLog;
    std::function<void(AsyncWebServerRequest *request)> testResultCallback;

    // Private helper functions
    void handleRootRequest(AsyncWebServerRequest *request);
    void handleStartTestRequest(AsyncWebServerRequest *request);
    void handleTestResultRequest(AsyncWebServerRequest *request);
    void handleLogRequest(AsyncWebServerRequest *request);
    void handleTestProgressRequest(AsyncWebServerRequest *request);
    void handleSubmitTestRequest(AsyncWebServerRequest *request);
    void handleGetTargetName(AsyncWebServerRequest *request);
        String generateLoadingPage();
};

#endif // WEBSERVERMANAGER_H
