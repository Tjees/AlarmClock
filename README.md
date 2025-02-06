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
    return ESP_OK;
}