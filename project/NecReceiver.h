// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "Display.h"
#include "InstelControl.h"      

// This file contains the code of multiple tasks that run concurrently and notify eachother using flags.

#define T_LEADSIGNAL_MIN_US 7000
#define T_LEADSIGNAL_MAX_US 11000
#define T_LEADPAUSE_MIN_US 3000
#define T_LEADPAUSE_MAX_US 6000
#define T_BITPAUSE_MIN_US 200
#define T_BITPAUSE_MAX_US 2000
#define T_BITPAUSE_THRESHOLD_ZERO_ONE 1100

namespace crt
{
	extern ILogger& logger;

	class NecReceiver : public Task
	{
        enum State
        {
            STATE_WAITING_FOR_LEAD_SIGNAL,
            STATE_WAITING_FOR_LEAD_PAUSE,
            STATE_WAITING_FOR_BIT_PAUSE
        };

	private:
        Queue<uint32_t, 10> signalQueue;
        Queue<uint32_t, 10> pauseQueue;
        State state;

        uint32_t t_signalUs;
        uint32_t t_pauseUs;

        uint32_t n;
        uint32_t m;

        uint32_t msg;
        uint32_t nofBytes;

        uint8_t byte1, byte2, byte3, byte4;

        InstelControl& instelControl;

	public:
		NecReceiver(const char *taskName, unsigned int taskPriority, unsigned int taskSizeBytes, unsigned int taskCoreNumber, InstelControl& instelControl) :	
			Task(taskName, taskPriority, taskSizeBytes, taskCoreNumber),
            signalQueue(this),
            pauseQueue(this),
            state(STATE_WAITING_FOR_LEAD_SIGNAL),
            instelControl(instelControl)
		{
			start();
		}

        void signalDetected(uint32_t t_Us) {
            if(t_Us > T_LEADSIGNAL_MIN_US) {
                signalQueue.clear();
                pauseQueue.clear();
            }
            signalQueue.write(t_Us);
            // ESP_LOGI("signal","%lu",t_Us);
        }

        void pauseDetected(uint32_t t_Us) {
            pauseQueue.write(t_Us);
            // ESP_LOGI("pause","%lu",t_Us);
        }

	private:
        void extractMessage (uint32_t& msg, uint32_t& nofBytes, uint32_t m, uint32_t n) {
            msg = 0;
            uint32_t mloc = m;
            for (int i = 0; i < n; i++) {
                msg = (msg << 1) | ((mloc >> i) & 1);
            }
            nofBytes = n/8;
        }

        void splitIntoHexBytes(uint32_t value) {
            byte1 = (value >> 24) & 0xFF;
            byte2 = (value >> 16) & 0xFF;
            byte3 = (value >> 8)  & 0xFF;
            byte4 = value & 0xFF;
        }

		/*override keyword not supported*/
		void main()
		{
			vTaskDelay(1000); // wait for other threads to have started up as well.

			while (true)
			{
				// dumpStackHighWaterMarkIfIncreased(); 		// This function call takes about 0.25ms! It should be called while debugging only.

				switch (state)
                {
                case STATE_WAITING_FOR_LEAD_SIGNAL:
                    signalQueue.read(t_signalUs);
                    if((t_signalUs > T_LEADSIGNAL_MIN_US) && (t_signalUs < T_LEADSIGNAL_MAX_US)) {
                        state = STATE_WAITING_FOR_LEAD_PAUSE;
                        logger.logText("Changed state to Waiting For Lead Pause.");
                    }
                    break;

                case STATE_WAITING_FOR_LEAD_PAUSE:
                    pauseQueue.read(t_pauseUs);
                    if((t_pauseUs > T_LEADPAUSE_MIN_US) && (t_pauseUs < T_LEADPAUSE_MAX_US)) {
                        n = 0;
                        m = 0;
                        state = STATE_WAITING_FOR_BIT_PAUSE;
                        logger.logText("Changed state to Waiting For Bit Pause.");
                    }
                    else {
                        state = STATE_WAITING_FOR_LEAD_SIGNAL;
                        logger.logText("Changed state to Waiting For Lead Signal.");
                    }
                    break;
                
                case STATE_WAITING_FOR_BIT_PAUSE:
                    pauseQueue.read(t_pauseUs);
                    if((t_pauseUs > T_BITPAUSE_MIN_US) && (t_pauseUs < T_BITPAUSE_MAX_US)) {
                        m = m<<1;
                        if(t_pauseUs > T_BITPAUSE_THRESHOLD_ZERO_ONE) {
                            m = m | 1;
                        }
                        n++;
                    }
                    else{
                        //extractMessage(msg, nofBytes, m, n);
                        splitIntoHexBytes(m);

                        ESP_LOGI("byte1", "%x", byte1);
                        ESP_LOGI("byte2", "%x", byte2);
                        ESP_LOGI("byte3", "%x", byte3);
                        ESP_LOGI("byte4", "%x", byte4);
                        ESP_LOGI("nofbytes","%lu", n);

                        state = STATE_WAITING_FOR_LEAD_SIGNAL;

                        logger.logText("Sent message to console.");
                        logger.logText("Changed state to Waiting For Lead Signal.");

                        // Geef byte3 door aan instelcontrol queue. 
                        instelControl.MessageReceived(byte3);
                    }
                    break;
                
                default:
                    break;
                }

                vTaskDelay(1);
                //taskYIELD();
			}
		}
	}; // end class BallControl

};// end namespace crt