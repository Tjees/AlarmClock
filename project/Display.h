// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "crt_FromArduinoIde.h"

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define SCREEN_ADDRESS 0x3C

// This file contains the code of multiple tasks that run concurrently and notify eachother using flags.

namespace crt
{
	extern ILogger& logger;

	class Display : public Task
	{
        enum State
        {
            STATE_IDLE
        };

	private:
        State state;
        Queue<uint8_t, 5> intQueue;
        uint8_t intToBeDisplayed;

        Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);;

	public:
		Display(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            state(STATE_IDLE),
            intQueue(this),
            intToBeDisplayed(0)
		{
			start();
		}

        void drawInt(uint8_t i) {
            intQueue.write(i);
        } 

	private:
        void drawNumber() {
            display.setTextSize(3);
            display.setTextColor(WHITE);
            display.setCursor(0,8);
            display.println(intToBeDisplayed);
            display.display();
            vTaskDelay(200);
            display.clearDisplay();
        }

		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

            if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
                    Serial.println(F("SSD1306 allocation failed"));
                    ESP_LOGI("dispayFailure", "failed to init display.");
                }
            display.clearDisplay();

			while (true)
			{
				switch (state)
                {
                case STATE_IDLE:
                    wait(intQueue);
                    intQueue.read(intToBeDisplayed);
                    drawNumber();
                    ESP_LOGI("display", "written to display");
                    break;
                
                default:
                    break;
                }

                taskYIELD();
		    }
	    }; // end class BallControl
    };
};// end namespace crt