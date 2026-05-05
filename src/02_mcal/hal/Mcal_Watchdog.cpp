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


static void (*s_watchdogCallBack) (void) = nullptr;

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
	/*If timeout is large, use the 256 prescaler
	  Without prescaler, max timeout is ~2048ms (65535 / 32) */
	if(timeout_ms > 2000u)
	{
		config.prescaler            = kRTWDOG_ClockPrescalerDivide256;
		config.workMode.enableDebug = true; //During debug, watchdog timer suspends
		/* Calculation: 32000 (Clock) / 256 (Prescalar) = 125
		 * timeout = timeout_ms * 125/1000
		 */
		config.timeoutValue         = (uint16_t)((timeout_ms * 125U) / 1000U);

	}
	else
	{
		// The RTWDOG uses clock cycles for its timeout.
		// If using the LPO (typically 32kHz), 1ms is ~32 ticks.
		config.timeoutValue = (timeout_ms * 32U);
	}
	config.enableInterrupt = true; //enable interrupt on watchdog reset

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

void Mcal::Watchdog::RegisterInterruptCallback(void (*cb)(void))
{
	s_watchdogCallBack = cb;
}


extern "C" void RTWDOG_IRQHandler(void)
{
	// 1. Clear Interrupt Flag (Required by hardware)
	RTWDOG_ClearStatusFlags(RTWDOG, kRTWDOG_InterruptFlag);

	//2. Execute the hook if one was registered
	if(s_watchdogCallBack != nullptr)
	{
		s_watchdogCallBack();
	}
	// 3. Wait for the hardware to perform the hard reset
	while(1);
}


