/**
 * @file svl_healthMonitor.hpp
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
#ifndef __SVL_HEALTHMONITOR_HPP_
#define __SVL_HEALTHMONITOR_HPP_

#include <stdint.h>

struct RTWDOG_PostMortemData {
	uint32_t magicNumber; 	// To verify the data validity after reset
	uint32_t pc;			// Program counter when it hung
	char taskName[20];		// Active task name
	uint32_t freeHeap;		// System health
	uint32_t heartBeatCount;//TODO: Use heartbeat ?
};

// Use the noinit attribute to ensure the linker places it correctly
extern RTWDOG_PostMortemData gRTWDOGPostMortemData __attribute__ ((section(".noinit")));

namespace Service
{
	class HealthMonitor
	{
		static void HandleFatalwatchdog(void);
		void Init(void);
	};
}

#endif /* __SVL_HEALTHMONITOR_HPP_ */
