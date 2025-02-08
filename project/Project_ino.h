// by Marius Versteegen, 2023

#include <crt_CleanRTOS.h>        // This file includes crt_Config.h  You'll need to change defines there for a release build.
#include <crt_Mutex.h>            // crt_Mutex.h must be included separately.

// All Tasks should be created in this main file.
#include <crt_Logger.h>
#include "SignalPauseDetector.h"
#include "NecReceiver.h"
#include "Display.h"

namespace crt
{
	// Create a "global" logger object withing namespace crt.
	const unsigned int pinButtonDump = 10; // Pressing a button connected to this pin dumps the latest logs to serial monitor.

	Logger<100> theLogger("Logger", 1 /*priority*/, ARDUINO_RUNNING_CORE, pinButtonDump);
	ILogger& logger = theLogger;	// This is the global object. It can be accessed without knowledge of the template parameter of theLogger.

    MainInits mainInits;            // Initialize CleanRTOS.
    
	Display oled("Display", 1 /*priority*/, 5000 /*stack size*/, ARDUINO_RUNNING_CORE);
    NecReceiver necReceiver("NecReceiver", 3 /*priority*/, 5000 /*stack size*/, ARDUINO_RUNNING_CORE);
    SignalPauseDetector pauseDetector("PauseDetector", 3 /*priority*/, 5000 /*stack size*/, ARDUINO_RUNNING_CORE, necReceiver);
}

void setup()
{
	// initialize serial communication at 115200 bits per second:
	// Serial.begin(115200);    // Only needed when using Serial.print();

	vTaskDelay(10);// allow tasks to initialize.
	ESP_LOGI("checkpoint", "start of main");
	vTaskDelay(1);
}

void loop()
{
	vTaskDelay(1);// Nothing to do in loop - all example code runs in the 4 threads above.
}