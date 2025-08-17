#pragma once
#include <cstdint>
#include <stdio.h>

class prj_Time {
    private:
        uint8_t hours;
        uint8_t minutes;
        uint8_t seconds;
        char str[20]; // it will probably never go over 9 characters 256:256:256.
        
    public:
        prj_Time(): hours(0), minutes(0), seconds(0) {
        }

    void setTime(std::array<uint8_t, 3> time) {
        hours = time[0];
        minutes = time[1];
        seconds = time[2];
    }

    std::array<uint8_t, 3> getTime() {
        return {hours, minutes, seconds};
    }

    // Bleh maar oke.
    void increaseTime() {
        seconds += 1;
        if(seconds > 59) {
            seconds = 0;
            minutes += 1;
            if(minutes > 59) {
                minutes = 0;
                hours += 1;
                if(hours > 23) {
                    hours = 0;
                }
            }
        }
    }

    void increaseMinutes() {
        minutes += 1;
        if(minutes > 59) {
            minutes = 0;
            hours += 1;
            if(hours > 23) {
                hours = 0;
            }
        }
    }

    void increaseHours() {
        hours += 1;
        if(hours > 23) {
            hours = 0;
        }
    }

    // This is pure shit change this.
    const char* getPrintableTime() {
        if(hours < 10 && minutes < 10 && seconds < 10) {
            sprintf(str, "0%u:0%u:0%u", hours, minutes, seconds);
        }
        else if(hours > 10 && minutes > 10 && seconds > 10) {
            sprintf(str, "%u:%u:%u", hours, minutes, seconds);
        }
        else if(hours < 10 && minutes > 10 && seconds > 10) {
            sprintf(str, "0%u:%u:%u", hours, minutes, seconds);
        }
        else if(hours > 10 && minutes < 10 && seconds > 10) {
            sprintf(str, "%u:0%u:%u", hours, minutes, seconds);
        }
        else if(hours > 10 && minutes > 10 && seconds < 10) {
            sprintf(str, "%u:%u:0%u", hours, minutes, seconds);
        }
        else if(hours > 10 && minutes < 10 && seconds < 10) {
            sprintf(str, "%u:0%u:0%u", hours, minutes, seconds);
        }
        else if(hours < 10 && minutes > 10 && seconds < 10) {
            sprintf(str, "0%u:%u:0%u", hours, minutes, seconds);
        }
        else if(hours < 10 && minutes < 10 && seconds > 10) {
            sprintf(str, "0%u:0%u:%u", hours, minutes, seconds);
        }
        // sprintf(str, "%u:%u:%u", hours, minutes, seconds);
        return str;
    }
};