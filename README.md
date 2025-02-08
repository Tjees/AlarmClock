Use this to install the Arduino component:
https://docs.espressif.com/projects/arduino-esp32/en/latest/esp-idf_component.html

In esp_timer.c change:

ESP_SYSTEM_INIT_FN(esp_timer_init_os, SECONDARY, ESP_TIMER_INIT_MASK, 100)
{
    return esp_timer_init();
}

To:
ESP_SYSTEM_INIT_FN(esp_timer_init_os, SECONDARY, ESP_TIMER_INIT_MASK, 100)
{
    esp_timer_init();
    return ESP_OK;
}

Probably cause rtos inits timers before startup so it doesnt start but idk just tried stuff. Some other methods didnt work so have to look at it later.

Also watch the buttons, configuring the wrong pins for the buttons can cause resets.

Tested with: esp32-s and esp32-C6.