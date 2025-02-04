#include "WebServerManager.h"

WebServerManager::WebServerManager(AsyncWebServer &server) : _server(server)
{
}
void WebServerManager::configureRoutes()
{
    _server.on("/", HTTP_GET, [this](AsyncWebServerRequest *request) { handleRootRequest(request); });

    _server.on("/start-test", HTTP_POST, [this](AsyncWebServerRequest *request) { handleStartTestRequest(request); });

    _server.on("/test-result", HTTP_GET, [this](AsyncWebServerRequest *request) { handleTestResultRequest(request); });
    // _server.on("/submit-test", HTTP_POST, [this](AsyncWebServerRequest *request) { handleSubmitTestRequest(request);
    // });

    _server.on("/get-target-name", HTTP_GET, [this](AsyncWebServerRequest *request) {
        String jsonResponse = "{\"targetName\":\"" + testRecord.uniqueName + "\"}";
        request->send(200, "application/json", jsonResponse);
    });
    _server.on("/save-results", HTTP_POST, [this](AsyncWebServerRequest *request) {
        Serial.println("Received POST request at /save-results");

        // Simulate saving results
        Serial.println("Saving test results...");

        // Send a success response
        request->send(200, "text/plain", "Results saved successfully.");
    });
    // New route to serve progress updates
    _server.on("/test-progress", HTTP_GET,
               [this](AsyncWebServerRequest *request) { handleTestProgressRequest(request); });

    // _server.on("/log", HTTP_GET, [this](AsyncWebServerRequest *request) { handleLogRequest(request); });

    _server.begin();
}
void WebServerManager::handleStartTestRequest(AsyncWebServerRequest *request)
{
    static unsigned long lastRequestTime = 0;
    unsigned long currentTime = millis();
    if (currentTime - lastRequestTime < 500)
    { // 500ms debounce
        request->send(429, "text/plain", "Too Many Requests");
        Serial.println("Many requests");

        return;
    }
    lastRequestTime = currentTime;

    if (onStartTest)
    {
        onStartTest();
    }

    isTestDataReady = false; // Reset test data readiness flag
    request->send(200, "text/plain", "Test started");
}
void WebServerManager::setTestResultCallback(std::function<void(AsyncWebServerRequest *request)> callback)
{
    testResultCallback = callback;
}

void WebServerManager::handleRootRequest(AsyncWebServerRequest *request)
{
    String html =
        "<html><head><title>TestKit IWS</title>"
        "<style>"
        "body {"
        "  font-family: Arial, sans-serif;"
        "  display: flex;"
        "  justify-content: center;"
        "  align-items: center;"
        "  height: 100vh;"
        "  margin: 0;"
        "  background-color: #f4f4f9;"
        "}"
        "#container {"
        "  text-align: center;"
        "  background: white;"
        "  padding: 20px;"
        "  border-radius: 10px;"
        "  box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);"
        "  width: 90%;"
        "  max-width: 500px;"
        "}"
        "h1 {"
        "  font-size: 24px;"
        "  color: #333;"
        "}"
        "p {"
        "  font-size: 16px;"
        "  color: #666;"
        "}"
        "button {"
        "  padding: 10px 20px;"
        "  font-size: 16px;"
        "  background-color: #3498db;"
        "  color: white;"
        "  border: none;"
        "  border-radius: 5px;"
        "  cursor: pointer;"
        "  margin-top: 20px;"
        "}"
        "button:hover {"
        "  background-color: #2980b9;"
        "}"
        "#animation {"
        "  width: 50px;"
        "  height: 50px;"
        "  border: 5px solid #ccc;"
        "  border-top: 5px solid #3498db;"
        "  border-radius: 50%;"
        "  animation: spin 1s linear infinite;"
        "  display: none;"
        "  margin: 20px auto;"
        "}"
        "@keyframes spin {"
        "  0% { transform: rotate(0deg); }"
        "  100% { transform: rotate(360deg); }"
        "}"
        "</style>"
        "</head><body>"

        "<div id='container'>"
        "<h1>TestKit IWS V2 Devices Interface</h1>"
        "<p>Please click the button below to start the test.</p>"

        // Start Test Button
        "<button id='startTest'>Start Test</button>"

        // Loading Animation
        "<div id='animation'></div>"

        // JavaScript Logic
        "<script>"
        "document.getElementById('startTest').addEventListener('click', function() {"
        "  console.log('Start Test button clicked.');"
        "  fetch('/start-test', { method: 'POST' })"
        "    .then(response => {"
        "      if (response.ok) {"
        "        console.log('POST request to /start-test successful.');"
        "        window.location.href = '/test-result';" // Redirect to /test-result"
        "      } else {"
        "        console.error('Failed to start the test. Response:', response.status);"
        "        alert('Failed to start the test. Please try again.');"
        "      }"
        "    })"
        "    .catch(error => {"
        "      console.error('Error sending POST request to /start-test:', error);"
        "      alert('An error occurred while starting the test. Please try again.');"
        "    });"
        "});"
        "</script>"

        "</div>" // Close container div
        "</body></html>";

    request->send(200, "text/html", html);
}

/*********************** START PROGRESS SCREEN */
void WebServerManager::handleTestProgressRequest(AsyncWebServerRequest *request)
{

    String response = "{ \"states\": [";
    switch (result_page)
    {
    case 0:
        response += "{ \"STEP\": \"Fetching test time\", \"status\": \"loading\" }";
        break;
    case 1:
        response += "{ \"STEP\": \"Got test time\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"doing DC signal test \", \"status\": \"loading\" }";
        break;
    case 2:
        response += "{ \"STEP\": \"Got test time\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"DC signal test\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"fetching target name\", \"status\": \"loading\" }";
        break;
    case 3:
        response += "{ \"STEP\": \"Got test time\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"DC signal test\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"target name fetched\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"MQTT toggle\", \"status\": \"loading\" }";
        break;
    case 4:
        response += "{ \"STEP\": \"Got test time\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"DC signal test\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"target name fetched\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"MQTT toggle\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"Saving in Google Sheet\", \"status\": \"loading\" }";
        break;
    case 5:
        response += "{ \"STEP\": \"Got test time\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"DC signal test\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"target name fetched\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"MQTT toggle\", \"status\": \"DONE\" },";
        response += "{ \"STEP\": \"Saved in Google Sheet\", \"status\": \"DONE\" }";
        break;
    }

    response += "],";

    if (result_page == TEST_COMPLETE)
    {

        response += "\"isComplete\": true,";
        response += "\"results\": {";
        response += "\"uniqueName\": \"" + testRecord.uniqueName + "\",";
        response += "\"local page\": \"<a href='http://" + testRecord.uniqueName + ".local/' target='_blank'>http://" +
                    testRecord.uniqueName + ".local/</a>\",";
        response += "\"Final Result\": \"" + finalResult.finalResult + "\",";
        response += "\"Test Time\": \"" + testRecord.testTime + "\",";
        response += "\"line1 Switch\": \"" + finalResult.rele1_toggle_signal + "\",";
        response += "\"line2 Switch\": \"" + finalResult.rele2_toggle_signal + "\",";
        response += "\"line1 toggled by MQTT\": \"" + finalResult.rele1_toggle_mqtt + "\",";
        response += "\"line2 toggled by MQTT\": \"" + finalResult.rele2_toggle_mqtt + "\",";
        response += "\"saved in google sheet\": \"" + finalResult.googlesheet_State + "\"";
        response += "}";
    }
    else if (result_page == MQTT_TIMEOUT)
    {
        finalResult.finalResult = "DEVICE IS OFLINE";
        response += "\"isComplete\": true,";
        response += "\"results\": {";
        response += "\"Final Result\": \"" + finalResult.finalResult + "\",";
        response += "\"Test Time\": \"" + testRecord.testTime + "\",";
        response += "\"line1 Switch\": \"" + finalResult.rele1_toggle_signal + "\",";
        response += "\"line2 Switch\": \"" + finalResult.rele2_toggle_signal + "\",";
        response += R"("MQTT": "can't reach")";
        response += "}";
    }
    else
    {
        response += "\"isComplete\": false";
    }

    response += "}";

    Serial.printf("\n[result_page] %d\n", result_page);
    // if (result_page >= 6)
    // {

    //     Serial.printf("Received GET request at /test-progress \n %s\n", response.c_str());
    // }

    request->send(200, "application/json", response);
}
void WebServerManager::handleTestResultRequest(AsyncWebServerRequest *request)
{
    Serial.println("Received GET request at /test-result");

    String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
        <meta charset="UTF-8">
        <title>Test Results</title>
        <style>
        
body {
  display: flex;
  justify-content: center;
  align-items: center;
  min-height: 100vh;
  min-height: 100dvh;
  background-color: #fff;
  transition-property: background-color;
  transition-duration: .2s;
}

.switch-container {
  position: relative;
  border-radius: 3.125em;
  box-shadow: 0 .125em .25em rgba(#000, .2);
  overflow: hidden;
  cursor: pointer;
}

.switch-input {
  -webkit-appearance: none;
  appearance: none;
  position: absolute;
  z-index: 1;
  width: 100%;
  height: 100%;
  opacity: 0;
  cursor: pointer;
}

.switch-button {
  display: inline-flex;
  padding: 0 .375em;
  border-radius: inherit;
  border: .0625em solid #6b717b;
  background-color: #aaafbb;
  box-shadow: inset 0 0 .5em rgba(#000, .4);
  transition-property: border-color, background-color;
  transition-duration: .2s;
  
  .switch-input:checked + & {
    border-color: #43316f;
    background-color: #7550d9;
  }
}

.switch-button-inside {
  display: inline-flex;
  column-gap: 1em;
  position: relative;
  border-radius: inherit;
  padding: .75em;
  transform: translateX(-.375em);
  background-image: linear-gradient(90deg, #c5c9d3 48%, #d5d7dd 52%);
  box-shadow: 
    inset .0625em 0 .0625em rgba(#fff, .4),
    inset -.0625em 0 .0625em rgba(#fff, .4);
  transition-property: transform;
  transition-duration: .2s;
  
  &::after {
    content: '';
    position: absolute;
    z-index: -1;
    inset: 0;
    border-radius: inherit;
    background-image: linear-gradient(90deg, #926afe 48%, #885bff 52%);
    box-shadow: 
      inset .0625em 0 .0625em rgba(#fff, .2),
      inset -.0625em 0 .0625em rgba(#fff, .2);
    opacity: 0;
    transition-property: opacity;
    transition-duration: .2s;
  }
  
  .switch-input:checked + .switch-button > & {
    transform: translateX(.375em);
    
    &::after {
      opacity: 1;
    }
  }
}

.switch-icon {
  width: 1em;
  height: 1em;
  fill: #fff;
  filter: 
    drop-shadow(0 .0625em .0625em rgba(#000, .4))
    drop-shadow(0 0 .25em rgba(#fff, .4))
    drop-shadow(0 0 .25em rgba(#fff, .4));
  transition-property: fill;
  transition-duration: .2s;
  
  &.off {
    .switch-input:checked + .switch-button & {
      fill: #704ccf;
      filter: none;
    }
  }
  
  &.on {
    .switch-input:not(:checked) + .switch-button & {
      fill: #767c86;
      filter: none;
    }
  }
}

            body {
                font-family: Arial, sans-serif;
                text-align: center;
                margin: 0;
                padding: 0;
                display: flex;
                justify-content: center;
                align-items: center;
                height: 100vh;
                background-color: #f4f4f9;
                box-sizing: border-box;
            }
            #container {
                text-align: center;
                background: white;
                padding: 20px;
                border-radius: 10px;
                box-shadow: 0 4px 10px rgba(0, 0, 0, 0.2);
                width: 90%;
                max-width: 600px;
            }
            table {
                width: 100%;
                border-collapse: collapse;
                margin: 10px 0;
            }
            table, th, td {
                border: 1px solid black;
                text-align: center;
                padding: 8px;
            }
            th {
                background-color: #3498db;
                color: white;
            }
        .loading svg {
            width: 30px;
            height: 20px;
        }
   
   .loading svg polyline {
       fill: none;
       stroke-width: 2;
       stroke-linecap: round;
       stroke-linejoin: round;
   }

   .loading svg polyline#back {
       stroke: rgba(255, 255, 255, 0.3);
       /* Changed to white for visibility */
   }

   .loading svg polyline#front {
       stroke: #00aaff;
       /* Replace with your desired color */
       stroke-dasharray: 12, 36;
       /* Dash 12 & Gap 36 */
       stroke-dashoffset: 48;
       animation: dash .5s linear infinite;
   }
      @keyframes dash {
       62.5% {
       opacity: 0;
       }

       100% {
           stroke-dashoffset: 0;
       }
   }
            td.done {
                color: #4caf50; /* Green color for DONE */
                font-weight: bold;
                animation: none; /* No blinking */
            }
            td.done::after {
                content: " ✔"; /* Unicode tick symbol */
                font-weight: bold;
                margin-left: 8px;
            }
            td.passed {
                color: #4caf50; 
                font-weight: bold;
            }
            td.fail {
                color: #e74c3c; 
                font-weight: bold;
            }
            td.default {
                color: #000; 
            }
            td.loading {
                color:rgb(255, 222, 34); 
                animation: blink 1s ;
            }
            @keyframes blink {
                0%, 100% { opacity: 1; }
                50% { opacity: 0.5; }
            }
            @keyframes rotate {
                0% { transform: rotate(0deg); }
                100% { transform: rotate(360deg); }
            }
            #loadingIndicator {
                margin-top: 20px;
                font-size: 18px;
                color: #ffa500;
                display: flex;
                align-items: center;
                justify-content: center;
                gap: 10px;
            }
            #spinner {
                width: 20px;
                height: 20px;
                border: 3px solid #ffa500;
                border-top: 3px solid transparent;
                border-radius: 50%;
                animation: rotate 1s linear infinite;
            }
            #buttonContainer {
                margin-top: 20px;
                display: none; /* Initially hide the buttons */
                flex-direction: column; /* Arrange buttons vertically */
                align-items: center;
                gap: 10px; /* Space between buttons */
            }
            button, a.button {
                padding: 10px 20px;
                font-size: 16px;
                color: white;
                text-decoration: none;
                border-radius: 5px;
                border: none;
                display: inline-block;
                cursor: pointer;
            }
            #saveAndGoBack {
                background-color: #3498db;
            }
            #reTest {
                background-color: #f39c12; /* Orange color for ReTEST */
            }
            #goToDatabase {
                background-color: #e91e63; /* Pink color for Go to Database */
            }
            #saveAndGoBack:hover, #reTest:hover, #goToDatabase:hover {
                opacity: 0.8;
            }
        </style>
    </head>
<body>
      <div id="container" style="display: flex; flex-direction: column; align-items: center; justify-content: center; height: 100vh;">
        <h1>Test Results</h1>
        
        <!-- Results table -->
        <div id="resultsTable"></div>
        
        <!-- Loading indicator directly after the table -->
        <div id="loadingIndicator" style="display: none; margin-top: 20px; text-align: center;">
            <div class="loading">
                <svg width="100px" height="100px">
                    <polyline id="back" points="5 10 10 10 15 20 25 5 28 10 30 10"></polyline>
                    <polyline id="front" points="5 10 10 10 15 20 25 5 28 10 30 10"></polyline>
                </svg>
            </div>
            <span>Test is still in progress...</span>
        </div>

        <!-- Buttons section -->
        <div id="buttonContainer" style="margin-top: 20px; display: none; flex-direction: column; align-items: center;">
            <!-- Relay Toggle Buttons Container -->
            <div style="display: flex; justify-content: center; gap: 20px; margin-bottom: 20px;">
                <!-- Relay 1 Toggle Button -->
                <div class="switch-container">
                    <label>Relay 1</label>
                    <input id="relay1Toggle" class="switch-input" type="checkbox">
                    <div class="switch-button">
                        <div class="switch-button-inside">
                            <svg class="switch-icon off" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg">
                                <path fill-rule="evenodd" clip-rule="evenodd" d="M8 12C10.2091 12 12 10.2091 12 8C12 5.79086 10.2091 4 8 4C5.79086 4 4 5.79086 4 8C4 10.2091 5.79086 12 8 12ZM8 14C11.3137 14 14 11.3137 14 8C14 4.68629 11.3137 2 8 2C4.68629 2 2 4.68629 2 8C2 11.3137 4.68629 14 8 14Z"/>
                            </svg>
                            <svg class="switch-icon on" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg">
                                <rect x="2" y="7" width="12" height="2" rx="1"/>
                            </svg>
                        </div>
                    </div>
                </div>

                <!-- Relay 2 Toggle Button -->
                <div class="switch-container">
                    <label>Relay 2</label>
                    <input id="relay2Toggle" class="switch-input" type="checkbox">
                    <div class="switch-button">
                        <div class="switch-button-inside">
                            <svg class="switch-icon off" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg">
                                <path fill-rule="evenodd" clip-rule="evenodd" d="M8 12C10.2091 12 12 10.2091 12 8C12 5.79086 10.2091 4 8 4C5.79086 4 4 5.79086 4 8C4 10.2091 5.79086 12 8 12ZM8 14C11.3137 14 14 11.3137 14 8C14 4.68629 11.3137 2 8 2C4.68629 2 2 4.68629 2 8C2 11.3137 4.68629 14 8 14Z"/>
                            </svg>
                            <svg class="switch-icon on" viewBox="0 0 16 16" xmlns="http://www.w3.org/2000/svg">
                                <rect x="2" y="7" width="12" height="2" rx="1"/>
                            </svg>
                        </div>
                    </div>
                </div>
            </div>

            <!-- Existing Buttons -->
            <div style="display: flex; gap: 10px;">
                <button id="saveAndGoBack">Save and Go Back</button>
                <button id="reTest">ReTEST</button>
                <a id="goToDatabase" class="button" href="add_yours" target="_blank">Go to Database</a>
            </div>
        </div>
    </div>
</body>
        <script>
/*******************************************************************/
        let targetName = "%TARGET_NAME%";

        // Fetch the target name from the server
        async function fetchTargetName() {
            try {
                let response = await fetch('/get-target-name');
                let data = await response.json();
                targetName = data.targetName;
                console.log("Received targetName:", targetName);
            } catch (error) {
                console.error('Error fetching target name:', error);
            }
        }

        // Toggle relay function
        async function toggleRelay(relayNumber, isChecked) {
            console.log(`Relay ${relayNumber} toggled to ${isChecked ? 'ON' : 'OFF'}`);
            await fetchTargetName();
            const relayURL = `http://${targetName}/switch/switch${relayNumber}_${targetName.substring(11)}/toggle`;
            try {
                const response = await fetch(relayURL, { method: 'POST' });
                if (!response.ok) throw new Error(`Failed to toggle Relay ${relayNumber}`);
                console.log(`Relay ${relayNumber} toggled successfully`);
            } catch (error) {
                console.error('Error toggling relay:', error.message);
            }
        }

        // Add event listeners to the relay toggles
        document.getElementById('relay1Toggle').addEventListener('change', function(event) {
            toggleRelay(1, event.target.checked);
        });

        document.getElementById('relay2Toggle').addEventListener('change', function(event) {
            toggleRelay(2, event.target.checked);
        });
    /*********************************************/
            function fetchAndDisplayResults() {
                fetch('/test-progress', { method: 'GET' })
                    .then(response => response.json())
                    .then(data => {
                        const tableDiv = document.getElementById('resultsTable');
                        const buttonContainer = document.getElementById('buttonContainer');
                        const loadingIndicator = document.getElementById('loadingIndicator');

                        if (!data.isComplete) {
                                                    buttonContainer.style.display = 'none'; // Show buttons when complete

                // relayToggleContainer.style.display = 'none'; // Show relay toggle container
                            loadingIndicator.style.display = 'flex'; // Show loading animation
                            let progressHTML = '<table><tr><th>Step</th><th>Status</th></tr>';
                            data.states.forEach(state => {
                                const statusClass = (state.status === "DONE") ? "done" : "loading";
                                progressHTML += `<tr><td>${state.STEP}</td><td class="${statusClass}">${state.status}</td></tr>`;
                            });
                            progressHTML += '</table>';
                            tableDiv.innerHTML = progressHTML;

                            setTimeout(fetchAndDisplayResults, 1000);
                        } else {
fetchTargetName();
                // relayToggleContainer.style.display = 'flex'; // Show relay toggle container

                            loadingIndicator.style.display = 'none'; // Hide loading animation
                            let resultsHTML = '<h2>Final Results</h2>';
                            resultsHTML += '<table><tr><th>Field</th><th>Value</th></tr>';
                            for (const [key, value] of Object.entries(data.results)) {
                                const statusClass =value.toLowerCase().includes("passed") ? "passed" : (value.toLowerCase().includes("fail") ? "fail" : "default");
                                resultsHTML += `<tr><td>${key}</td><td class="${statusClass}">${value}</td></tr>`;
                            }
                            resultsHTML += '</table>';
                            tableDiv.innerHTML = resultsHTML;

                            buttonContainer.style.display = 'flex'; // Show buttons when complete
                        }
                    })
                    .catch(error => {
                        console.error('Error fetching test results:', error);
                        document.getElementById('resultsTable').innerHTML = `<p>Error loading test results: ${error.message}</p>`;
                    });
            }

            // Initial call to fetch and display results
            fetchAndDisplayResults();

            // Handle "Save and Go Back" button click
            document.getElementById('saveAndGoBack').addEventListener('click', function() {
                fetch('/save-results', { method: 'POST' })
                    .then(response => {
                        if (response.ok) {
                            window.location.href = '/';
                        } else {
                            throw new Error('Save failed');
                        }
                    })
                    .catch(error => alert('Failed to save: ' + error.message));
            });

            // Handle "ReTEST" button click
            document.getElementById('reTest').addEventListener('click', function() {
                fetch('/start-test', { method: 'POST' })
                    .then(response => {
                        if (response.ok) {
                            // alert('Test restarted successfully');
                            fetchAndDisplayResults();
                        } else {
                            throw new Error('Failed to restart the test');
                        }
                    })
                    .catch(error => alert('Failed to restart test: ' + error.message));
            });
        </script>
    </body>
    </html>
    )rawliteral";
    html.replace("%TARGET_NAME%", testRecord.uniqueName);

    request->send(200, "text/html", html);
}

/*********************** END PROGRESS SCREEN */

// void WebServerManager::handleLogRequest(AsyncWebServerRequest *request)
// {
//     if (request == nullptr)
//     {
//         Serial.println("Error: Invalid request pointer.");
//         return;
//     }

//     String html = _htmlGenerator.generateLogHTML(_testLog);
//     request->send(200, "text/html", html);
// }
