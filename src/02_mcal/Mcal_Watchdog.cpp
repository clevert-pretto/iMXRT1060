/**
 * @file Mcal_Watchdog.cpp
 * @author ravic
 * @brief TODO
 * @date 18-Apr-2026
 * * @copyright Copyright (c) 2026 ravic. All rights reserved.
 * * @attention
 * This software is the confidential and proprietary information of ravic.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Use, distribution, or modification of this code is permitted only under 
 * the terms of the formal license agreement provided by the copyright holder.
 */

#include "Mcal_Watchdog.hpp"

extern "C" {
	#include "fsl_rtwdog.h"
	#include "fsl_common.h"
}

#if defined(__ICCARM__) || defined(__CC_ARM) || defined(__GNUC__)
    extern uint32_t SystemCoreClock;
#endif

void Mcal::Watchdog::Initialize(uint32_t timeout_ms)
{
	rtwdog_config_t config;

	/* * RTWDOG_GetDefaultConfig sets:
	 * - Enable Watchdog: true
	 * - Clock Source: LPO (Low Power Osc)
	 * - Window Mode: Disabled
	 * - Prescaler: 1
	 */
	RTWDOG_GetDefaultConfig(&config);

	config.enableRtwdog = true;

	// The RTWDOG uses clock cycles for its timeout.
	// If using the LPO (typically 32kHz), 1ms is ~32 ticks.
	config.timeoutValue = (timeout_ms * 32U);

	RTWDOG_Init(RTWDOG, &config);

}


void Mcal::Watchdog::Refresh(void)
{
	RTWDOG_Refresh(RTWDOG);
}

void Mcal::Watchdog::TriggerReset(void)
{
	// To test the reset path, we can simply disable the watchdog
	// without the proper unlock sequence, or wait for timeout.
	// Most safety tests intentionally "hang" here to verify the hardware reset.
	while(1);
}


