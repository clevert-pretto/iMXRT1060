/**
 * @file Mcal_post.hpp
 * @author ravic
 * @brief TODO
 * @date 22-Apr-2026
 * * @copyright Copyright (c) 2026 ravic. All rights reserved.
 * * @attention
 * This software is the confidential and proprietary information of ravic.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Use, distribution, or modification of this code is permitted only under 
 * the terms of the formal license agreement provided by the copyright holder.
 */
#ifndef __MCAL_POST_HPP_
#define __MCAL_POST_HPP_

#include <cstdint>

namespace Mcal {

	/**
     * @brief Result codes for Phase 1 POST.
     * Each bit represents a specific failure for easy bitmask reporting.
     */
	enum class PostStatus : uint32_t {
		enumPASS = 0,
		enumCpuFail     = (1 << 0),
		enumTempFail    = (1 << 1),
		enumFlexRamFail = (1 << 2),
		enumRamFail     = (1 << 3),
		enumFlashFail   = (1 << 4),
		enumRevisionFail= (1 << 5)
	};

	/**
	 * @brief Reset cause captured from the SRC (system reset controller) hardware
	 */
	enum class ResetCause : uint32_t {
		enumPowerOn,
		enumWatchdog,
		enumLockup,
		enumJTAG,
		enumCSU,
		enumTempSense,
		enumUnknown
	};

	class Post {
	public:
		// Phase 1: Critical Core Tests
		static PostStatus RunCpuAluTest();             // Test 1
		static PostStatus VerifyBoardRevision();       // Test 2
		static PostStatus CheckInternalTemp();         // Test 3
		static PostStatus VerifyFlexRamPartitioning(); // Test 4 & 6
		static PostStatus RunInternalRamIntegrity();   // Test 5
		static PostStatus VerifyFlashIntegrity();      // Test 7 & 8
		static ResetCause GetResetCause();             // Test 9
	};
}







#endif /* __MCAL_POST_HPP_ */
