#ifndef MDNS_MANAGER_H
#define MDNS_MANAGER_H

#include <Arduino.h>
#include <ESPmDNS.h>


class MDNSManager
{
  public:
    MDNSManager(const char *hostname);
    void start();

  private:
    const char *_hostname;
};

#endif // MDNS_MANAGER_H
