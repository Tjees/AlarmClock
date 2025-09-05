// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "NecReceiver.h"
#include "TsopReceiver.h"
#include "crt_FromArduinoIde.h"
#include "freertos/event_groups.h"

// This file contains the code of multiple tasks that run concurrently and notify eachother using flags.

#define T_MAX_PAUSE_US 6000
#define MIN_SIGNAL_US 300
#define MIN_PAUSE_US 300
#define MAX_SIGNAL_US 12000
#define MAX_PAUSE_US 8000

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

        int64_t t_startTime;
        int64_t t_stopTime;

        uint32_t duration;

        NecReceiver& necReceiver;
        TsopReceiver tsopReceiver = TsopReceiver(11);

	public:
        static crt::SignalPauseDetector* instance;
        Flag signalFlag;

		SignalPauseDetector(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, NecReceiver& necReceiver) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            timer(this),
            state(STATE_WAITING_FOR_SIGNAL),
            t_signalUs(0),
            t_pauseUs(0),
            t_startTime(0),
            t_stopTime(0),
            duration(0),
            necReceiver(necReceiver),
            signalFlag(this)
		{
            instance = this;
            t_startTime = esp_timer_get_time();
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
                case STATE_WAITING_FOR_PAUSE:
                    timer.start(12000); // if no signal within this time, reset state machine.
                    //logger.logText("WAITING_FOR_PAUSE");
                    waitAny(signalFlag + timer);
                    if(hasFired(timer)) {
                        state = STATE_WAITING_FOR_SIGNAL;
                    }
                    else if (hasFired(signalFlag)) {
                        t_stopTime = esp_timer_get_time();
                        duration = t_stopTime - t_startTime;
                        t_startTime = t_stopTime;

                        logger.logInt32(duration);

                        necReceiver.signalDetected((uint32_t)duration);
                        timer.start(7000);
                        state = STATE_WAITING_FOR_SIGNAL;
                    }
                    break;

                case STATE_WAITING_FOR_SIGNAL:
                    //logger.logText("WAITING_FOR_PAUSE");
                    waitAny(signalFlag + timer);
                    if(hasFired(timer)) {
                        necReceiver.pauseDetected(7000);
                    }
                    else if(hasFired(signalFlag)) {
                        t_stopTime = esp_timer_get_time();
                        duration = t_stopTime - t_startTime;
                        t_startTime = t_stopTime;

                        logger.logInt32(duration);

                        necReceiver.pauseDetected((uint32_t)duration);
                        state = STATE_WAITING_FOR_PAUSE;
                    }
                    break;
                
                default:
                    break;
                }

                taskYIELD();
                //vTaskDelay(1);
			}
		}
	}; // end class BallControl

};// end namespace crt