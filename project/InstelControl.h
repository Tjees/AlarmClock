// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "crt_FromArduinoIde.h"
#include "freertos/event_groups.h"
#include "Display.h"
#include "prj_Time.h"
#include "TijdInstelControl.h"
#include "AlarmInstelControl.h"

// This file contains the code of multiple tasks that run concurrently and notify eachother using flags.

// In de taakstructurering hebben InstelControl, TijdInstelControl en AlarminstelControl dezelfde periode.
// Hierdoor voeg ik deze samen in een taak genaamd InstelControl.
// Ook zijn het instellen van tijd en alarm afhankelijk van het menu vandaar dat dit een taak is geworden.

#define T_MAX_PAUSE_US 6000

namespace crt
{
	extern ILogger& logger;

	class InstelControl : public Task
	{
        enum State
        {
            WAITING_FOR_MESSAGE,
            MENU_OPENED,
            TIME_SETTING_MENU,
            ALARM_SETTING_MENU
        };

	private:
        State state;

        Queue<uint8_t, 5> messageQueue;
        uint8_t byte;

        // flag etc.
        Timer timer;
        Display &display;

        prj_Time &time;
        prj_Time &alarm;

        uint32_t buzzTime;

        TijdInstelControl tijdInstelControl;
        AlarmInstelControl alarmInstelControl;

	public:
        InstelControl(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, Display& display, prj_Time &time, prj_Time &alarm) :	
            Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            state(WAITING_FOR_MESSAGE),
            messageQueue(this),
            byte(0),
            timer(this),
            display(display),
            time(time),
            alarm(alarm),
            buzzTime(0),
            tijdInstelControl(this, display, time, messageQueue),
            alarmInstelControl(this, display, alarm, messageQueue)
		{
			start();
		}

        void MessageReceived(uint8_t byte) {
            messageQueue.write(byte);
        }

        Queue<uint8_t, 5>& getMessageQueue() {
            return messageQueue;
        }

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				// dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				switch (state)
                {
                case WAITING_FOR_MESSAGE:
                    waitAny(messageQueue);
                    if(hasFired(messageQueue)) {
                        messageQueue.read(byte);
                        if(byte == 2) {
                            state = MENU_OPENED;
                        }
                        else {
                            state = WAITING_FOR_MESSAGE;
                        }
                    }
                    break;

                case MENU_OPENED:
                    display.showMenu();
                    waitAny(messageQueue);
                    if(hasFired(messageQueue)) {
                        messageQueue.read(byte);
                        if(byte == 104) {
                            state = TIME_SETTING_MENU;
                        }
                        else if(byte == 176) {
                            state = ALARM_SETTING_MENU;
                        }
                        else {
                            display.closeMenu();
                            state = WAITING_FOR_MESSAGE;
                        }
                    }
                    break;

                case TIME_SETTING_MENU: {
                    int response = tijdInstelControl.ChangeTimeSetting();
                    if(response == 1) {
                        display.closeMenu();
                        state = WAITING_FOR_MESSAGE;
                    }
                    break;
                }

                case ALARM_SETTING_MENU: {
                    int response = alarmInstelControl.ChangeAlarmSetting();
                    if(response == 1) {
                        display.closeMenu();
                        state = WAITING_FOR_MESSAGE;
                    }
                    break;
                }
                
                default:
                    break;
                }

                taskYIELD();
			}
		}
	}; // end class BallControl

};// end namespace crt