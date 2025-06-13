// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "crt_FromArduinoIde.h"
#include "freertos/event_groups.h"
#include "Display.h"
#include "prj_Time.h"

// This file contains the code of multiple tasks that run concurrently and notify eachother using flags.

#define T_MAX_PAUSE_US 6000

namespace crt
{
	extern ILogger& logger;

	class OperationControl : public Task
	{
        enum State
        {
            STATE_UPDATE_TIME,
            STATE_SOUND_ALARM
        };

	private:
        State state;

        // flag etc.
        Timer timer;
        Display &display;
        Mutex& oledMutex;
        prj_Time &time;
        uint32_t buzzTime;

	public:
		OperationControl(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, Display& display, Mutex& oledMutex, prj_Time &time) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            state(STATE_UPDATE_TIME),
            timer(this),
            display(display),
            oledMutex(oledMutex),
            time(time),
            buzzTime(0)
		{
			start();
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
                case STATE_UPDATE_TIME:
                    vTaskDelay(1000);
                    if(false) {
                        state = STATE_SOUND_ALARM;
                    }
                    else {
                        time.increaseTime();
                        display.drawString(time.getPrintableTime());
                    }
                    break;

                case STATE_SOUND_ALARM:
                    timer.sleep_us(500);
                    buzzTime += 500;
                    if(buzzTime == 3000000) {
                        state = STATE_UPDATE_TIME;
                    }
                    break;
                
                default:
                    break;
                }

                // taskYIELD();
                // vTaskDelay(1000); // wait 1 second.
			}
		}
	}; // end class BallControl

};// end namespace crt