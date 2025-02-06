// by Marius Versteegen, 2023

// MainInits initializes CleanRTOS.
// A MainInits object must be defined before the Tasks are started.

#pragma once
#include "internals/crt_FreeRTOS.h"
namespace crt
{
    class MainInits
    {
    public:
        MainInits()
        {
            // New esp idf inits timers automatically. It throws an error when initializing twice. Currently its turned off.
            esp_timer_init();   // Allow creation and use of microsecond timers.
        }
    };
};
