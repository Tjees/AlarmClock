# lasergame-template

This template is meant as a quick startup repo for students that start with a lasergame project on ESP32. 

These are the steps that should be followed:

## Install ESP-IDF

Select a fast drive (like nvme disk) with to install the ESP-IDF on (it takes about 50GB after building) (building thousands of small files, drive speed often is the bottleneck).

- Download and execute the latest offline ESP-IDF installer of version 4 (at time of writing: v4.4.5) from:
  [dl.espressif.com/dl/esp-idf/](https://dl.espressif.com/dl/esp-idf/)
  When asked, check all checkboxes for maximum support of esp32 variations.

- Pin the powershell that opens to the desktop.

## Teamleader only: create team repo for the lasergame

- Teamleader creates a new, **empty** **private** repo named lasergame on Github (without .gitignore, readme.md and whatever).

- Teamleader adds team members as collaborators.

- Teamleader clones this template to a local folder:
  git clone "https://github.com/mavehu/lasergame-template.git" lasergame

- cd lasergame

- git remote remove origin

- git remote add origin "https://github.com/teamleadergitname/lasergame.git"

- git push -u origin master

## Other team members clone the team repo

The other team members get their own clone of the team repo:

- git clone "https://github.com/teamleadergitname/lasergame.git" lasergame

## All team members import the arduino ide component

To be able to use libraries that were built for arduino ide in the ESP-IDF project, the arduino ide component for ESP32 needs to be added:

- cd lasergame

- cd components

Then git clone the arduino-esp32 branch associated with ESP-IDF 4.4.
Today, that's:

- git clone -b idf-release/v4.4 https://github.com/espressif/arduino-esp32.git

## Test

- Open the ESP-IDF powershell

- cd to the lasergame folder that contains the team repo

- connect the ESP32 devkit to your computer

- check which new com port appeared in windows device manager or in Arduino IDE (port)

- Type (in my case its COM4)
  idf.py -p COM4 flash monitor
  **but before** pressing enter, hold the boot button of the ESP32 till flashing starts (otherwise, an error might be thrown).

- After flashing, you should see "HelloWorld: Hello World!" repeatedly.

- Abort the serial monitor with CTRL+]

## Other useful ESP-IDF commands

- For building without flashing:
  idf.py build

- For cleaning up in for complete rebuild:
  idf.py fullclean

- For changing settings (careful):
  idf.py menuconfig

- For changing build target for different ESP32 types:
  idf.py set-target ESP32s2
  idf.py set-target ESP32s3

Note that when ESP32s2 or ESP32s3 is used, you could use them as "mouse/joystick/rudder-devices",
but in that case, you'd need ESP-IDF version 5.x (and the corresponding branch of the arduino-esp32 component).

## Other important pitfalls on ESP32 to be aware of

- When using the ESP32s2 or EPS32s3 as an usb device (joystick, mouse, keyboard), first uncomment next line in main/idf_component.yml:
  #espressif/esp_tinyusb: "~1.0.0"
- Find (online) the **pinout diagram** of the specific ESP32 devkit that you ar using. 
- Note a "pin number" on the diagram is not the same as "gpio pin number" and "devkit pin number". It stands for the pin of of the microcontroller chip on the board.
- Note that part of the gpio-pins (gpio 34 up till 39) can be used as input pin only.
- A large part of the pins cannot be used safely at all, because they are already used by background processes on the esp32.
  - Don't use during boot: pin 0,2,5,12,14,15 (must be NC during boot).
  - Don't use 1, 9, 10, 11, 16 and 17.
  - Pin1 (tx0) is already used for debug output.
  - Pin3 (rx0) could be used as input pin.
- At this moment (oktober 2023), Wifi examples of the arduino-esp32 component yield an error during execution. The solution is: add next initialization code:

```cpp
// In main.cpp:

#include "nvs_flash.h" // nodig voor WIFI functionaliteit

void app_main(void)
{
    // Initialisatie van NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());  // Wis de NVS-partitie en probeer opnieuw
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    //.. etc - rest van app_main
```
