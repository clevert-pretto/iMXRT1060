/**
 * @file svl_healthMonitor.cpp
 * @author ravic
 * @brief TODO
 * @date 26-Apr-2026
 * * @copyright Copyright (c) 2026 ravic. All rights reserved.
 * * @attention
 * This software is the confidential and proprietary information of ravic.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Use, distribution, or modification of this code is permitted only under 
 * the terms of the formal license agreement provided by the copyright holder.
 */

#include "svl_healthMonitor.hpp"
#include "Mcal_Watchdog.hpp"
extern "C"
{
	#include "fsl_debug_console.h"
	#include "FreeRTOS.h"
	#include "task.h"
}
RTWDOG_PostMortemData gRTWDOGPostMortemData;
/* Handle RTWDOG interrupt */

namespace Service
{
	void HealthMonitor::HandleFatalwatchdog()
	{
		// This is the actual logic you wanted to write
		// Capture 'last gasp' data into your .noinit structure
		gRTWDOGPostMortemData.magicNumber = 0xDEADBEEF;
		gRTWDOGPostMortemData.freeHeap = xPortGetFreeHeapSize();

		TaskHandle_t xHandle = xTaskGetCurrentTaskHandle();
		if(xHandle != NULL) {
			// Accessing OS-specific data safely in the Service layer
			const char* name = pcTaskGetName(xHandle);
			strncpy(gRTWDOGPostMortemData.taskName, name, 20);
		}

		// Synchronous print bypassing the logger task
		DbgConsole_Printf("\r\n[FATAL] WDOG Timeout in Task: %s\r\n",
							  gRTWDOGPostMortemData.taskName);
	}

	void HealthMonitor::Init()
	{
		Mcal::Watchdog::RegisterInterruptCallback(HealthMonitor::HandleFatalwatchdog);
	}
}
