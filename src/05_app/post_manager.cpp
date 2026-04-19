/**
 * @file post_manager.cpp
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

#include "Mcal_Watchdog.hpp"

namespace App {

	bool RunSafetyPost()
	{
		//1. Initialize watchdog with a 2 second timerout
		Mcal::Watchdog::Initialize(2000);

		//2. Perform a test

		//3. can we refresh it now?
		Mcal::Watchdog::Refresh();


		//4. Log the status (through UART wrapper)


		return true;
	}
}


