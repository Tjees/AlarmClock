// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "crt_FromArduinoIde.h"
#include "prj_Time.h"

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
            WAITING_FOR_EVENT,
            DRAW_TIME,
            DRAW_MENU,
            DRAW_EDIT_TIME
        };

	private:
        State state;

        Queue<String, 1> currentTimeChannel;
        Queue<String, 1> editTimeChannel;
        Flag showMenuFlag;
        Flag closeMenuFlag;

        String currentTime;
        String editTime;

        Adafruit_SSD1306 display = Adafruit_SSD1306(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

	public:
		Display(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            state(WAITING_FOR_EVENT),
            currentTimeChannel(this),
            editTimeChannel(this),
            showMenuFlag(this),
            closeMenuFlag(this)
		{
			start();
		}

        void showCurrentTime(String time) {
            currentTimeChannel.write(time);
        }

        void showEditTime(String time) {
            editTimeChannel.write(time);
        }

        void showMenu() {
            showMenuFlag.set();
        }

        void closeMenu() {
            closeMenuFlag.set();
        }

	private:
        void drawString(String string) {
            display.clearDisplay();

            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(0,8);
            display.println(string);
            display.display();
        }

        void drawMenu() {
            display.clearDisplay();

            display.setTextSize(2);
            display.setTextColor(WHITE);
            display.setCursor(0,8);
            display.println("===MENU===");
            display.println("1. Set time");
            display.println("2. Set alarm");
            display.display();
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
                case WAITING_FOR_EVENT:
                    waitAny(currentTimeChannel + showMenuFlag);
                    if(hasFired(currentTimeChannel)) {
                        currentTimeChannel.read(currentTime);
                        state = DRAW_TIME;
                    }
                    else if(hasFired(showMenuFlag)) {
                        state = DRAW_MENU;
                    }
                    
                    break;
                
                case DRAW_TIME:
                    drawString(currentTime);
                    state = WAITING_FOR_EVENT;
                    break;

                case DRAW_MENU:
                    drawMenu();
                    waitAny(editTimeChannel + closeMenuFlag);
                    if(hasFired(editTimeChannel)) {
                        editTimeChannel.read(editTime);
                        state = DRAW_EDIT_TIME;
                    }
                    else if(hasFired(closeMenuFlag)) {
                        state = WAITING_FOR_EVENT;
                    }
                    break;

                case DRAW_EDIT_TIME:
                    drawString(editTime);
                    waitAny(editTimeChannel + closeMenuFlag);
                    if(hasFired(editTimeChannel)) {
                        editTimeChannel.read(editTime);
                        state = DRAW_EDIT_TIME;
                    }
                    else if(hasFired(closeMenuFlag)) {
                        state = WAITING_FOR_EVENT;
                    }
                    break;
                
                default:
                    break;
                }

                taskYIELD();
		    }
	    }; // end class BallControl
    };
};// end namespace crt