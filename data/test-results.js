   function fetchAndDisplayResults() {

//     const loadingIndicator = document.getElementById('loading');

// if (loadingIndicator) {
//     loadingIndicator.style.display = 'flex'; // Show loading animation
// } else {
//     console.error('Loading indicator element not found');
// }

        fetch('/test-progress', { method: 'GET' })
            .then(response => response.json())
            .then(data => {
                const tableDiv = document.getElementById('resultsTable');
                const buttonContainer = document.getElementById('buttonContainer');
                const loadingIndicator = document.getElementById('loading');

                if (!data.isComplete) {
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
                    loadingIndicator.style.display = 'none'; // Hide loading animation
                    let resultsHTML = '<h2>Final Results</h2>';
                    resultsHTML += '<table><tr><th>Field</th><th>Value</th></tr>';
                    for (const [key, value] of Object.entries(data.results)) {
                        const statusClass = value.toLowerCase().includes("passed") ? "passed" :
                            (value.toLowerCase().includes("fail") ? "fail" : "default");
                        resultsHTML += `<tr><td>${key}</td><td class="${statusClass}">${value}</td></tr>`;
                    }
                    resultsHTML += '</table>';

                    // Add buttons in the same line
                    resultsHTML += `
                                <div id="relayButtonsContainer" 
                                style="margin-top: 20px; display: flex; justify-content: center; gap: 60px;">

                                <!-- Relay 1 -->
                                <div style="display: flex; flex-direction: column; align-items: center;">
                                    <input type="checkbox" id="relay1" style="display:none;">
                                    <label for="relay1" 
                                            style="position: relative; 
                                                width: 100px; 
                                                height: 50px; 
                                                background: linear-gradient(0deg, #121720, #0d1217);
                                                border-radius: 50px;
                                                box-shadow: 0 0 5px #0006, 0 -4px 10px #0b0b10, 0 0 10px #b9e1ff88, 0 -4px 20px #15182fcc;
                                                cursor: pointer;">
                                        <div class="thumb" 
                                            style="position: absolute; 
                                                width: 45px; 
                                                height: 45px; 
                                                top: 2px; 
                                                left: 3px; 
                                                background: radial-gradient(circle at 48% 50%, #fff0 60%, #03A9F4 100%), 
                                                            linear-gradient(180deg, #283349, #17212f);
                                                border-radius: 50%;
                                                transition: all 0.3s ease;">
                                        </div>
                                    </label>
                                    <div class="light" id="relay1LED" 
                                        style="margin-top: 10px;
                                            width: 20px; 
                                            height: 20px; 
                                            border-radius: 50%; 
                                            background: gray;
                                            box-shadow: 0 0 10px rgba(0, 0, 0, 0.6);
                                            transition: all 0.3s ease;">
                                    </div>
                                </div>

                                <!-- Relay 2 -->
                                <div style="display: flex; flex-direction: column; align-items: center;">
                                    <input type="checkbox" id="relay2" style="display:none;">
                                    <label for="relay2" 
                                            style="position: relative; 
                                                width: 100px; 
                                                height: 50px; 
                                                background: linear-gradient(0deg, #121720, #0d1217);
                                                border-radius: 50px;
                                                box-shadow: 0 0 5px #0006, 0 -4px 10px #0b0b10, 0 0 10px #b9e1ff88, 0 -4px 20px #15182fcc;
                                                cursor: pointer;">
                                        <div class="thumb" 
                                            style="position: absolute; 
                                                width: 45px; 
                                                height: 45px; 
                                                top: 2px; 
                                                left: 3px; 
                                                background: radial-gradient(circle at 48% 50%, #fff0 60%, #03A9F4 100%), 
                                                            linear-gradient(180deg, #283349, #17212f);
                                                border-radius: 50%;
                                                transition: all 0.3s ease;">
                                        </div>
                                    </label>
                                    <div class="light" id="relay2LED" 
                                        style="margin-top: 10px;
                                            width: 20px; 
                                            height: 20px; 
                                            border-radius: 50%; 
                                            background: gray;
                                            box-shadow: 0 0 10px rgba(0, 0, 0, 0.6);
                                            transition: all 0.3s ease;">
                                    </div>
                                </div>
                            </div>
                            `;
                    tableDiv.innerHTML = resultsHTML;

                    buttonContainer.style.display = 'flex'; // Show buttons when complete
                    fetchTargetName();
                    // // Add event listeners for the new buttons
                    // Add the new fetch-based toggle functionality:

                    document.getElementById('relay1').addEventListener('change', function () {
                        updateLED('relay1LED', this.checked);
                        toggleRelay(1, this.checked);
                    });

                    document.getElementById('relay2').addEventListener('change', function () {
                        updateLED('relay2LED', this.checked);
                        toggleRelay(2, this.checked);
                    });
                    async function toggleRelay(relayNumber, isChecked) {
                        console.log(`Relay ${relayNumber} toggled to ${isChecked ? 'ON' : 'OFF'}`);

                        try {
                            // Fetch the target name dynamically
                            await fetchTargetName();

                            // Construct the API URL based on the relay number
                            const relayURL = `http://${targetName}/switch/switch${relayNumber}_${targetName.substring(11)}/toggle`;

                            // Send a POST request to toggle the relay
                            const response = await fetch(relayURL, {
                                method: 'POST'
                            });

                            if (response.ok) {
                                console.log(`Relay ${relayNumber} toggled successfully`);
                            } else {
                                throw new Error(`Failed to toggle Relay ${relayNumber}. Status: ${response.status}`);
                            }
                        } catch (error) {
                            console.error('Error toggling relay:', error.message);
                            alert('Error: ' + error.message);
                        }
                    }
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
    document.getElementById('saveAndGoBack').addEventListener('click', function () {
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

    document.getElementById('reTest').addEventListener('click', function () {
        fetch('/start-test', { method: 'POST' })
            .then(response => {
                if (response.ok) {
                    fetchAndDisplayResults();
                } else {
                    throw new Error('Failed to restart the test');
                }
            })
            .catch(error => alert('Failed to restart test: ' + error.message));
    });
    fetchAndDisplayResults();
    // function fetchTargetName() {
    //     fetch('/get-target-name')
    //         .then(response => response.json())
    //         .then(data => {
    //             targetName = data.targetName;
    //             console.log("Received targetName:", targetName);
    //         })
    //         .catch(error => console.error('Error fetching target name:', error));
    // }
    let targetName = "%TARGET_NAME%";

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

    function updateLED(ledId, isChecked) {
        const led = document.getElementById(ledId);
        led.style.background = isChecked ? 'limegreen' : 'gray';
        led.style.boxShadow = isChecked ? '0 0 15px rgba(0, 255, 0, 0.8)' : '0 0 10px rgba(0, 0, 0, 0.6)';
    }