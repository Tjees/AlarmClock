#include <Arduino.h>

class TsopReceiver {
    private:
        uint8_t pinNumber;

    public:
        TsopReceiver(uint8_t pinNumber): pinNumber(pinNumber) {
            pinMode(pinNumber, INPUT);
        }

        bool isSignalPresent() {
            if(digitalRead(pinNumber) == LOW) {
                // ESP_LOGI("received", "received a signal");
                return true;
            }
            else {
                return false;
            }
        }
};