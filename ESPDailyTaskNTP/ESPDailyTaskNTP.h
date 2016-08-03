/*
ESPdailyTask library for ESP8266


This routine gets the unixtime from a NTP server and adjusts it to the time zone and the
Middle European summer time if requested
   
Copyright (c) 2016 Andreas Spiess

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

V1.0 2016-8-3

*/

#ifndef _ESP_DAILY_TASK_H
#define _ESP_DAILY_TASK_H

#include <Arduino.h>
#include <SNTPtime.h>


class ESPDailyTaskNTP {

  public:
    ESPDailyTaskNTP(int hours,int minutes, char *mySSID, char *myPASSWORD);
    ESPDailyTaskNTP(int hours,int minutes, char *mySSID, char *myPASSWORD, byte RESET_PIN);

    void sleepOneDay();
    void backToSleep();

  private:
    void backToSleep(boolean wifiOn);
    void printRtcMem(String place);
    void processCurrentTime(int time);
    int adjustTime();
};

#endif // _ESP_DAILY_TASK_H
