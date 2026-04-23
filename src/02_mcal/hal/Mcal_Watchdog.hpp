/**
 * @file Mcal_Watchdog.hpp
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
#ifndef __MCAL_WATCHDOG_HPP__
#define __MCAL_WATCHDOG_HPP__

#pragma once
#include <stdint.h>

namespace Mcal {

	class Watchdog {
	public:
		//Windowed watchdog for safety-critical ssytem
		static void Initialize(uint32_t timeout_ms);
		static void Refresh();
		static void TriggerReset(); //for testing the reset path
	};
} //namespace Mcal


#endif /* __MCAL_WATCHDOG_HPP__ */
