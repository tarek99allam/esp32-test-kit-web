#include "MDNSManager.h"

MDNSManager::MDNSManager(const char* hostname) : _hostname(hostname) {}

void MDNSManager::start() {
    if (!MDNS.begin("testkit-iws"))
    {
        Serial.println("*************************************");
        Serial.println("*        Error starting mDNS        *");
        Serial.println("*************************************");
        return;
    }

    Serial.println("\n\n=============================");
    Serial.println("    DNS is now available     ");
    Serial.println("=============================");
    Serial.println("    Access  testkit   at:    ");
    Serial.println("    http://testkit-iws.local  ");
    Serial.println("=============================\n\n");
}
