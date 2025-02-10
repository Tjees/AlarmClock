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

In the Adafruit display library add some yields to switch to tasks during displaying. Like:

  taskYIELD(); // yield to allow other tasks to run after sending i2c data.
  uint16_t count = WIDTH * ((HEIGHT + 7) / 8);
  uint8_t *ptr = buffer;
  if (wire) { // I2C
    wire->beginTransmission(i2caddr);
    WIRE_WRITE((uint8_t)0x40);
    uint16_t bytesOut = 1;
    while (count--) {
      if (bytesOut >= WIRE_MAX) {
        wire->endTransmission();
        taskYIELD(); // yield again after ending transmission ( so one block/chunk of pixels ).
        wire->beginTransmission(i2caddr);
        WIRE_WRITE((uint8_t)0x40);
        bytesOut = 1;
      }
      WIRE_WRITE(*ptr++);
      bytesOut++;
    }
    wire->endTransmission();
  } else { // SPI
    SSD1306_MODE_DATA
    while (count--)
      SPIwrite(*ptr++);
  }
  TRANSACTION_END
  taskYIELD(); // yield to allow other tasks to run after sending i2c data.