// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "NecReceiver.h"
#include "TsopReceiver.h"
#include "crt_FromArduinoIde.h"

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

        NecReceiver& necReceiver;
        TsopReceiver tsopReceiver = TsopReceiver(11);

	public:
		SignalPauseDetector(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, NecReceiver& necReceiver) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            timer(this),
            state(STATE_WAITING_FOR_PAUSE),
            t_signalUs(0),
            necReceiver(necReceiver)
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
                case STATE_WAITING_FOR_PAUSE:
                    timer.sleep_us(100);
                    //delayMicroseconds(100);
                    if(tsopReceiver.isSignalPresent()) {
                        t_signalUs += 100;
                        // ESP_LOGI("increase", "increased signal us.");
                        // ESP_LOGI("signal","%lu",t_signalUs);
                    }
                    else {
                        necReceiver.signalDetected(t_signalUs);
                        t_pauseUs = 0;
                        state = STATE_WAITING_FOR_SIGNAL;
                        logger.logUint32(t_signalUs);
                    }
                    break;

                case STATE_WAITING_FOR_SIGNAL:
                    timer.sleep_us(100);
                    //delayMicroseconds(100);
                    if(!tsopReceiver.isSignalPresent()) {
                        t_pauseUs += 100;
                        if(t_pauseUs > T_MAX_PAUSE_US) {
                            necReceiver.pauseDetected(t_pauseUs);
                            t_pauseUs = 0;
                        }
                    }
                    else {
                        necReceiver.pauseDetected(t_pauseUs);
                        t_signalUs = 0;
                        state = STATE_WAITING_FOR_PAUSE;
                        // logger.logText("Changed state since pause detected.");
                    }
                    break;
                
                default:
                    break;
                }
			}
            
            taskYIELD();
		}
	}; // end class BallControl

};// end namespace crt