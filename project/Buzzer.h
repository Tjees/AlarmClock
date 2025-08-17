// by Marius Versteegen, 2024

#pragma once
#include <Arduino.h>
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

	class Buzzer : public Task
	{
        enum State
        {
            WAITING_FOR_EVENT,
            BUZZING
        };

	private:
        State state;

        Timer timer;
        Flag buzzFlag;

        uint32_t buzzTime;
        uint8_t pinNumber;

	public:
		Buzzer(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, uint8_t pinNumber) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            state(WAITING_FOR_EVENT),
            timer(this),
            buzzFlag(this),
            buzzTime(2000000),
            pinNumber(pinNumber)
		{
			start();
		}

        void makeSound() {
            buzzFlag.set();
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
                case WAITING_FOR_EVENT:
                    wait(buzzFlag);
                    state = BUZZING;
                    break;
                case BUZZING:
                    digitalWrite(pinNumber, HIGH); // Turn on buzzer.
                    timer.sleep_us(buzzTime); // Wait for buzzTime.
                    digitalWrite(pinNumber, LOW); // Turn off buzzer.
                    buzzFlag.clear(); // Clear the flag.
                    state = WAITING_FOR_EVENT;
                    break;
                    
                default:
                    break;
                }

                taskYIELD();
			}
		}
	}; // end class BallControl

};// end namespace crt