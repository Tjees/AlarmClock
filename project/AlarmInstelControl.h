// by Marius Versteegen, 2024

#pragma once
#include <crt_CleanRTOS.h>
#include "crt_FromArduinoIde.h"
#include "freertos/event_groups.h"
#include "Display.h"
#include "prj_Time.h"

namespace crt
{
	extern ILogger& logger;

	class AlarmInstelControl
	{
        enum State
        {
            WAITING_FOR_MESSAGE,
            EDITING_MINUTES,
            EDITING_HOURS
        };

	private:
        State state;

        Display& display;

        prj_Time& currentAlarm;
        prj_Time editAlarm;

        uint8_t byte;

        Task* m_pTask;
        Queue<uint8_t, 5>& messageQueue;

	public:
        AlarmInstelControl(Task* m_pTask, Display& display, prj_Time& currentAlarm, Queue<uint8_t, 5>& messageQueue) : 
            state(WAITING_FOR_MESSAGE),
            display(display),
            currentAlarm(currentAlarm),
            editAlarm(currentAlarm),
            byte(0),
            m_pTask(m_pTask),
            messageQueue(messageQueue)
		{}

        int ChangeAlarmSetting() {
            switch (state)
            {
            case WAITING_FOR_MESSAGE:
                display.showEditTime(editAlarm.getPrintableTime());
                m_pTask->waitAny(messageQueue);
                if(m_pTask->hasFired(messageQueue)) {
                    messageQueue.read(byte);
                    if(byte == 90) {
                        state = EDITING_MINUTES;
                    }
                    else if(byte == 16) {
                        state = EDITING_HOURS;
                    }
                    else {
                        return 1;
                    }
                }
                break;
            
            case EDITING_MINUTES:
                display.showEditTime(editAlarm.getPrintableTime());
                m_pTask->waitAny(messageQueue);
                if(m_pTask->hasFired(messageQueue)) {
                    messageQueue.read(byte);
                    if(byte == 24) {
                        editAlarm.increaseMinutes();
                    }
                    else if(byte == 74)
                    {
                        //decrease minutes;
                    }
                    else if(byte == 56) {
                        currentAlarm.setTime(editAlarm.getTime());
                        state = WAITING_FOR_MESSAGE;
                        return 1; // this tells the caller to return to the main menu.
                    }
                    else if(byte == 16) {
                        state = EDITING_HOURS;
                    }
                    else {
                        state = EDITING_MINUTES;
                    }
                }
                break;

            case EDITING_HOURS:
                display.showEditTime(editAlarm.getPrintableTime());
                m_pTask->waitAny(messageQueue);
                if(m_pTask->hasFired(messageQueue)) {
                    messageQueue.read(byte);
                    if(byte == 24) {
                        editAlarm.increaseHours();
                    }
                    else if(byte == 74)
                    {
                        //decrease minutes;
                    }
                    else if(byte == 56) {
                        currentAlarm.setTime(editAlarm.getTime());
                        state = WAITING_FOR_MESSAGE;
                        return 1; // this tells the caller to return to the main menu.
                    }
                    else if(byte == 90) {
                        state = EDITING_MINUTES;
                    }
                    else {
                        state = EDITING_HOURS;
                    }
                }
                break;
            
            default:
                break;
            }

            return 0;
        }
	}; // end class BallControl

};// end namespace crt