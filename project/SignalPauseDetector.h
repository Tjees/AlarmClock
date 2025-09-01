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
#define MAX_SIGNAL_US 30000
#define MAX_PAUSE_US 70009

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
            state(STATE_WAITING_FOR_PAUSE),
            t_signalUs(0),
            t_pauseUs(0),
            t_startTime(0),
            t_stopTime(0),
            duration(0),
            necReceiver(necReceiver),
            signalFlag(this)
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
                    //logger.logText("WAITING_FOR_PAUSE");
                    waitAny(signalFlag);
                    if(hasFired(signalFlag)) {
                        t_stopTime = esp_timer_get_time();
                        duration = (uint32_t)t_stopTime - t_startTime;
                        t_startTime = t_stopTime;
                        if(duration > MIN_SIGNAL_US && duration < MAX_SIGNAL_US) {
                            logger.logUint32(duration);
                            necReceiver.signalDetected(duration);
                            timer.start( T_MAX_PAUSE_US + 1000 ); // +1000 to avoid too early firing.);
                            state = STATE_WAITING_FOR_SIGNAL;
                        }
                    }
                    break;

                case STATE_WAITING_FOR_SIGNAL:
                    //logger.logText("WAITING_FOR_SIGNAL");
                    waitAny(signalFlag + timer);
                    if(hasFired(signalFlag)) {
                        t_stopTime = esp_timer_get_time();
                        duration = (uint32_t)t_stopTime - t_startTime;
                        t_startTime = t_stopTime;
                        if(duration > MIN_PAUSE_US && duration < MAX_PAUSE_US) {
                            logger.logUint32(duration);
                            necReceiver.pauseDetected(duration);
                            state = STATE_WAITING_FOR_PAUSE;
                        }
                    }
                    else if(hasFired(timer)) {
                        necReceiver.pauseDetected( T_MAX_PAUSE_US + 1000 );
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