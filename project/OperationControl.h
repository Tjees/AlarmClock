// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "crt_FromArduinoIde.h"
#include "freertos/event_groups.h"
#include "Display.h"
#include "prj_Time.h"
#include "Buzzer.h"

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
        Display& display;
        Buzzer& buzzer;

        prj_Time &time;
        prj_Time &alarm;

	public:
		OperationControl(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, Display& display, Buzzer& buzzer, prj_Time &time, prj_Time &alarm) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            state(STATE_UPDATE_TIME),
            timer(this),
            display(display),
            buzzer(buzzer),
            time(time),
            alarm(alarm)
		{
			start();
		}

	private:
		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

            timer.start_periodic(1000000); // 1 second timer.
			while (true)
			{
				// dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				switch (state)
                {
                case STATE_UPDATE_TIME:
                    wait(timer); // changed to timer interrupt, now the alarm state can set the buzzer, go back and hopefully not miss the next timer interrupt. ( or the state takes longer than one second but then that is that... although setting a flag doesnt take nearly as much time as the timer callback does, so it should be fine. )
                    time.increaseTime();
                    display.showCurrentTime(time.getPrintableTime());
                    if(alarm.getTime() == time.getTime()) {
                        state = STATE_SOUND_ALARM;
                    }
                    else {
                        state = STATE_UPDATE_TIME;
                    }
                    break;

                case STATE_SOUND_ALARM:
                    buzzer.makeSound();
                    state = STATE_UPDATE_TIME;
                    break;
                
                default:
                    break;
                }

                taskYIELD();
			}
		}
	}; // end class BallControl

};// end namespace crt