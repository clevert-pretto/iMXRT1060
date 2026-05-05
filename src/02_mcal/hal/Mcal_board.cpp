/**
 * @file Mcal_board.cpp
 * @author ravic
 * @brief TODO
 * @date 19-Apr-2026
 * * @copyright Copyright (c) 2026 ravic. All rights reserved.
 * * @attention
 * This software is the confidential and proprietary information of ravic.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Use, distribution, or modification of this code is permitted only under 
 * the terms of the formal license agreement provided by the copyright holder.
 */

#include "Mcal_board.hpp"
#include "Mcal_Watchdog.hpp"
#include "sysConfig.hpp" //app system specific macros
extern "C" {
	#include "pin_mux.h"
	#include "clock_config.h"
	#include "board.h"
	#include "fsl_device_registers.h"
	#include "fsl_debug_console.h"
}

void Mcal::Board::InitHardware(void)
{
	BOARD_ConfigMPU();
	BOARD_InitBootPins();
	BOARD_InitBootClocks();

	/* Enable DWT Cycle Counter for SDK_Delay functions */
	CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
	DWT->CYCCNT = 0;
	DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;

	Mcal::Watchdog::Initialize(SYS_WATCHDOG_TIMEOUT_ms);

	BOARD_InitDebugConsole();
}
