#ifndef TIME_SYNC_MANAGER_H
#define TIME_SYNC_MANAGER_H

#include <Arduino.h>
#include <time.h>


class TimeSyncManager
{
  public:
    TimeSyncManager(const char *ntpServer, long gmtOffsetSec, int daylightOffsetSec);
    void synchronize();
    String getCurrentTime();

  private:
    const char *_ntpServer;
    long _gmtOffsetSec;
    int _daylightOffsetSec;
};

#endif // TIME_SYNC_MANAGER_H
