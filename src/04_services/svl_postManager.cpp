/**
 * @file svl_postManager.cpp
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

#include "svl_postManager.hpp"
#include "Svl_logger.hpp"
#include "Mcal_post.hpp"

namespace Service {


	// POST - Reset Forensic first
	Mcal::ResetCause PostManager::RunPhase1ResetCauseTest (void)
	{
		return (Mcal::Post::GetResetCause());
	}

	uint32_t PostManager::RunPhase1CriticalCoreTest (void)
	{
		uint32_t ret = 0;

		ret |= static_cast<uint32_t>(Mcal::Post::RunCpuAluTest());
		ret |= static_cast<uint32_t>(Mcal::Post::VerifyBoardRevision());
		ret |= static_cast<uint32_t>(Mcal::Post::CheckInternalTemp());
		ret |= static_cast<uint32_t>(Mcal::Post::VerifyFlexRamPartitioning());
		ret |= static_cast<uint32_t>(Mcal::Post::RunInternalRamIntegrity());
		ret |= static_cast<uint32_t>(Mcal::Post::VerifyFlashIntegrity());

		return ret;
	}

	bool PostManager::RunPhase2InternalPeriBusTest (void)
	{
		bool ret = false;

		return ret;
	}
	bool PostManager::RunPhase3InternalSysAcceltest (void)
	{
		bool ret = false;

		return ret;
	}
	bool PostManager::RunPhase4ExternalFunctionalTest (void)
	{
		bool ret = false;

		return ret;
	}
}


