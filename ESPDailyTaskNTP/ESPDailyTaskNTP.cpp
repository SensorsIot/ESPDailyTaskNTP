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

#include "ESPDailyTaskNTP.h"

extern "C" {
  #include "user_interface.h" // this is for the RTC memory read/write functions
}

SNTPtime NTPch("ch.pool.ntp.org");

#define OFF 0
#define ON 1

const unsigned long ONE_SECOND = 1000000;
const unsigned long ONE_HOUR =  3600*ONE_SECOND;
//const unsigned long ONE_HOUR =  2000000;
//= 60 * 60 * ONE_SECOND;  // number of microseconds (for deep_sleep of one hour)

enum statusDef {
        RESET,
        COUNTING,
        CHECK,
        WORK
};

typedef struct {
        byte markerFlag;
        byte counter;
        statusDef status;
} rtcStore __attribute__((aligned(4))); 
rtcStore rtcMem;


int wakeUpTime;
byte _resetPin;
bool wifiOn;
char *_ssid, *_password;
unsigned long _sleepTime;

unsigned long thisSleepTime;

// Desired wake up time
strDateTime _wakeup, _actualTime;



//Constructor
ESPDailyTaskNTP::ESPDailyTaskNTP(int hours,int minutes, char *mySSID, char *myPASSWORD, byte RESET_PIN){
        _wakeup.hour = hours;
        _wakeup.minute=minutes;
        _wakeup.second = 0;
        _resetPin=RESET_PIN;
        _ssid = mySSID;
        _password=myPASSWORD;
        if (_resetPin!=99) pinMode(_resetPin,INPUT_PULLUP);
}

//Constructor
ESPDailyTaskNTP::ESPDailyTaskNTP(int hours,int minutes, char *mySSID, char *myPASSWORD){

   byte RESET_PIN=99;

}

/**
 * The deepSleep time is an uint32 of microseconds so a maximum of 4,294,967,295, about 71 minutes.
 * So to publish a reading once per day needs to sleep for one hour 24 times and keep track of
 * where its up to in RTC memory. The WiFi radio is switched off for all but the 24th wakeup
 * to save power
 */
void ESPDailyTaskNTP::sleepOneDay() {

        int _secondsToWait;
        
          unsigned long entry=millis();

        system_rtc_mem_read(65, &rtcMem, sizeof(rtcMem));
        if ((_resetPin!=99 && digitalRead(_resetPin)==0 )|| rtcMem.markerFlag!=85) rtcMem.status=RESET;

        switch (rtcMem.status) {

        case RESET:
                rtcMem.markerFlag = 85;
                rtcMem.counter = 0;
                _sleepTime=1;
                rtcMem.status = CHECK;
                system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
                printRtcMem("RESET  ");
                ESP.deepSleep(_sleepTime, WAKE_RF_DEFAULT);
                break;

        case COUNTING:
                if (rtcMem.counter==0) {
                        _sleepTime=1;
                        rtcMem.status=CHECK;
                        system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
                        printRtcMem("COUNTING ZERO ");
                        ESP.deepSleep(_sleepTime, WAKE_RF_DEFAULT);
                }
                else {
                        rtcMem.counter--;
                        _sleepTime=ONE_HOUR;
                        system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
                        printRtcMem("COUNTING DOWN  ");
                        ESP.deepSleep(_sleepTime, WAKE_RF_DISABLED);
                }
                Serial.print("This call took ");
                Serial.print(millis()-entry);
                Serial.println(" milliseconds");
                break;

        case CHECK:
                _secondsToWait = adjustTime();
                if (_secondsToWait>120) {
                        if (_secondsToWait>3600) {
                                rtcMem.counter = (int)(_secondsToWait/3600);
                                rtcMem.status=COUNTING;
                                _sleepTime=ONE_HOUR;
                                system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
                                printRtcMem("CHECK  ");
                                ESP.deepSleep(_sleepTime, WAKE_RF_DISABLED);
                        }
                        else {
                                rtcMem.status=WORK;
                                _sleepTime=_secondsToWait*ONE_SECOND;
                                system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
                                printRtcMem("CHECK AND WAIT FOR WORK  ");
                                ESP.deepSleep(_sleepTime, WAKE_RF_DEFAULT);
                        }
                }
                else {
                        rtcMem.status=WORK;
                        _sleepTime=1;
                        system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
                        printRtcMem("CHECK 3  ");
                        ESP.deepSleep(_sleepTime, WAKE_RF_DEFAULT);
                }
                break;

        case WORK:
                break;

        }
}

int ESPDailyTaskNTP::adjustTime() {
        long _currentSecs,_wakeUpSecs;
        int _seconds;
        unsigned long entry=millis();
                WiFi.begin (_ssid, _password);
                while (WiFi.status() != WL_CONNECTED && millis()-entry<10000) {
                        Serial.print(".");
                        delay(500);
                }
        if (millis()-entry>10000) ESP.deepSleep(1, WAKE_RF_DEFAULT);  // no connection possible, try again
        Serial.println("WiFi connected");

        entry=millis();
        while(!NTPch.setSNTPtime()&& millis()-entry<10000) Serial.println("x");
        _actualTime = NTPch.getTime(1.0, true);
        NTPch.printDateTime(_actualTime);
        if (millis()-entry<10000) {
                _currentSecs = ((_actualTime.hour * 60) + _actualTime.minute) * 60 + _actualTime.second;
                _wakeUpSecs = ((_wakeup.hour * 60) + _wakeup.minute) * 60 + _wakeup.second;
                _seconds=(_wakeUpSecs-_currentSecs>0) ? (_wakeUpSecs-_currentSecs) : (_wakeUpSecs-_currentSecs+(24*3600));
                Serial.printf("_currentSecs: %3d\n",_currentSecs);
                Serial.printf("_wakeUpSecs: %3d\n",_wakeUpSecs);
                Serial.printf("_seconds: %3d\n",_seconds);
                return _seconds;
        } else ESP.deepSleep(1, WAKE_RF_DEFAULT);
}

void ESPDailyTaskNTP::printRtcMem(String place) {
        Serial.print(place);
        Serial.print(" ");
        //    Serial.print("rtc marker: ");
        //    Serial.print(rtcMem.markerFlag);
        Serial.print("Status: ");
        Serial.print(rtcMem.status);
        Serial.print(", markerFlag: ");
        Serial.print(rtcMem.markerFlag);
        Serial.print(", counter: ");
        Serial.print(rtcMem.counter);
        Serial.print(", sleepTime: ");
        Serial.print(_sleepTime);
        Serial.print(", wifiOn: ");
        Serial.print(wifiOn);
        Serial.println();
}


void ESPDailyTaskNTP::backToSleep() {
                rtcMem.counter = 23;
                _sleepTime=ONE_HOUR;
                rtcMem.status=COUNTING;
                system_rtc_mem_write(65, &rtcMem, sizeof(rtcMem));
                printRtcMem("WORK  ");
        ESP.deepSleep(_sleepTime, WAKE_RF_DISABLED);
}