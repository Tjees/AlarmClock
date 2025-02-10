// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "NecReceiver.h"
#include "TsopReceiver.h"
#include "crt_FromArduinoIde.h"
#include "freertos/event_groups.h"

// This file contains the code of multiple tasks that run concurrently and notify eachother using flags.

#define T_MAX_PAUSE_US 6000

namespace crt
{
	extern ILogger& logger;

	class SignalPauseDetector : public Task
	{
        enum State
        {
            STATE_WAITING_FOR_PAUSE,
            STATE_WAITING_FOR_SIGNAL
        };

	private:
        Timer timer;
        State state;

        uint32_t t_signalUs;
        uint32_t t_pauseUs;

        uint32_t t_startTime;
        uint32_t t_stopTime;

        NecReceiver& necReceiver;
        TsopReceiver tsopReceiver = TsopReceiver(11);

	public:
        static crt::SignalPauseDetector* instance;
        Flag signalFlag;
        Flag pauseFlag;

		SignalPauseDetector(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, NecReceiver& necReceiver) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            timer(this),
            state(STATE_WAITING_FOR_SIGNAL),
            t_signalUs(0),
            t_startTime(0),
            necReceiver(necReceiver),
            signalFlag(this),
            pauseFlag(this)
		{
            instance = this;
			start();
		}

        // static IRAM_ATTR void setSignalFlag(void* args) {
        //     if(instance) {
        //         instance -> t_signalUs = 100;
        //     }
        //     portYIELD_FROM_ISR();
        // }

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
                case STATE_WAITING_FOR_PAUSE:
                    wait(pauseFlag);
                    t_stopTime = esp_timer_get_time();
                    necReceiver.signalDetected(t_stopTime - t_startTime);
                    timer.start(6000);
                    t_startTime = esp_timer_get_time();
                    state = STATE_WAITING_FOR_SIGNAL;
                    break;

                case STATE_WAITING_FOR_SIGNAL:
                    waitAny(signalFlag + timer);
                    if(hasFired(signalFlag)) {
                        t_stopTime = esp_timer_get_time();
                        necReceiver.pauseDetected(t_stopTime - t_startTime);
                        t_startTime = esp_timer_get_time();
                        state = STATE_WAITING_FOR_PAUSE;
                    }
                    else if(hasFired(timer)) {
                        necReceiver.pauseDetected(T_MAX_PAUSE_US);
                    }
                    break;
                
                default:
                    break;
                }

                taskYIELD();
                // vTaskDelay(1);
			}
		}
	}; // end class BallControl

};// end namespace crt