#include "TimeSyncManager.h"

TimeSyncManager::TimeSyncManager(const char* ntpServer, long gmtOffsetSec, int daylightOffsetSec)
    : _ntpServer(ntpServer), _gmtOffsetSec(gmtOffsetSec), _daylightOffsetSec(daylightOffsetSec) {}

void TimeSyncManager::synchronize() {
    configTime(_gmtOffsetSec, _daylightOffsetSec, _ntpServer);
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        Serial.println("Failed to obtain time");
    } else {
        Serial.println("Time synchronized");
    }
}

String TimeSyncManager::getCurrentTime() {
    struct tm timeinfo;
    if (!getLocalTime(&timeinfo)) {
        return "Time not available";
    }
    char timeString[20];
    strftime(timeString, sizeof(timeString), "%Y-%m-%d %H:%M:%S", &timeinfo);
    return String(timeString);
}
