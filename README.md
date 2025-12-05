# ESPDailyTaskNTP

[![MIT License](https://img.shields.io/badge/License-MIT-yellow.svg)](https://opensource.org/licenses/MIT)
[![GitHub stars](https://img.shields.io/github/stars/SensorsIot/ESPDailyTaskNTP)](https://github.com/SensorsIot/ESPDailyTaskNTP/stargazers)

![ESP8266](https://img.shields.io/badge/ESP8266-Supported-green?logo=espressif)
![Arduino](https://img.shields.io/badge/Arduino-Compatible-00979D?logo=arduino)

Library for ESP8266 to execute a task once per day at a specified time using NTP synchronization and deep sleep.

## 📦 Dependencies

Install these libraries first:

| Library | Description |
|---------|-------------|
| [SNTPtime](https://github.com/SensorsIot/SNTPtime) | SNTP time synchronization |

## 📥 Installation

1. Download this repository as ZIP
2. Arduino IDE → Sketch → Include Library → Add .ZIP Library
3. Install the SNTPtime dependency (link above)

## ⚙️ Configuration

### 📶 WiFi Credentials

**Option 1:** Define credentials directly in your sketch (recommended):

```cpp
char my_SSID[] = "your-wifi-name";
char my_PASSWORD[] = "your-wifi-password";
```

**Option 2:** Create a `credentials.h` file in your Arduino libraries folder:

```cpp
char my_SSID[] = "your-wifi-name";
char my_PASSWORD[] = "your-wifi-password";
```

Then include it with `#include <credentials.h>`.

### 🔌 Wiring

Connect a reset pin (e.g., D2) to RST for deep sleep wake-up:

| ESP8266 Pin | Connection |
|-------------|------------|
| D2 (GPIO4)  | RST        |

## 💡 Usage

```cpp
#include <ESP8266WiFi.h>
#include <ESPDailyTaskNTP.h>
#include <SNTPtime.h>

char my_SSID[] = "your-wifi";
char my_PASSWORD[] = "your-password";

#define RESET_PIN D2

// Wake up at 12:00, timezone +1.0 (CET)
ESPDailyTaskNTP dailyTask(12, 0, 1.0, my_SSID, my_PASSWORD, RESET_PIN);

void setup() {
  Serial.begin(115200);

  dailyTask.sleepOneDay();

  // Your daily task code here
  Serial.println("Doing daily task...");

  dailyTask.backToSleep();
}

void loop() {
  // Never reached - device is sleeping
}
```

## 📁 Examples

| Example | Description |
|---------|-------------|
| `OncePerDayNTP` | Execute task once per day with NTP sync |
