/*

OncePerDay example for ESP8266
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

#include <ESP8266WiFi.h>
#include <ESPDailyTaskNTP.h>
#include <SNTPtime.h>

SNTPtime NTPwork("ch.pool.ntp.org");

#ifdef D2
#define RESET_PIN D2
#else
#define RESET_PIN 4
#endif

#define my_SSID "YOUR_SSID"
#define my_PASSWORD "YOUR_WIFI_PASSWORD"

ESPDailyTaskNTP dailyTask(13, 9, 1, my_SSID, my_PASSWORD, RESET_PIN); // Hour to do the task

void setup() {
  Serial.begin(115200);
  Serial.println();   Serial.println("Start");   Serial.println();
  
  dailyTask.sleepOneDay();

  // ------------------ put the code for your daily task here -------------------------------

  Serial.println("............ W O R K ...............................");
  NTPwork.setSNTPtime();
  strDateTime _time = NTPwork.getTime(1.0, 1);
  NTPwork.printDateTime(_time);

  sendSparkfun(1, "AndreasSpiess", _time.year, _time.month, _time.day, _time.hour, _time.minute);
  // ----------------------- end of code for your daily task-------------------------------

  // and back to sleep once daily code is done
  dailyTask.backToSleep();
}

void loop() {
  // sleeping so wont get here
}
