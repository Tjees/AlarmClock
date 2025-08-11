// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "crt_FromArduinoIde.h"
#include "freertos/event_groups.h"
#include "Display.h"
#include "prj_Time.h"

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
            STATE_IDLE,
            STATE_EDITING_MINUTES,
            STATE_EDITING_HOURS
        };

	private:
        State state;

        // flag etc.
        Timer timer;
        Display &display;
        prj_Time &time;
        uint32_t buzzTime;

	public:
        InstelControl(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, Display& display, prj_Time &time) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            state(STATE_UPDATE_TIME),
            timer(this),
            display(display),
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
                case STATE_IDLE:
                    vTaskDelay(1000);
                    break;

                case STATE_SOUND_ALARM:
                    vTaskDelay(1000);
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