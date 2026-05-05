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
		bool retVal = false;
		uint32_t results = 0;

		results |= static_cast<uint32_t>(Mcal::Post::VerifyClockStability());
		results |= static_cast<uint32_t>(Mcal::Post::CheckPowerRails());
		results |= static_cast<uint32_t>(Mcal::Post::VerifyWatchdogLatching());
		results |= static_cast<uint32_t>(Mcal::Post::RunSdramTest()); //Failing due to debug
		results |= static_cast<uint32_t>(Mcal::Post::VerifyMpuRegions());
		results |= static_cast<uint32_t>(Mcal::Post::TestNvicSoftwareInterrupt());
		results |= static_cast<uint32_t>(Mcal::Post::VerifyI2cBusHealth());
		results |= static_cast<uint32_t>(Mcal::Post::CheckBeeStatus());
		results |= static_cast<uint32_t>(Mcal::Post::CalibrateAdc());

		if(results == 0)
		{
			Service::Logger::Log(LogLevel::enumInfo, "[POST] Phase 2: Internal System & MPU Health PASSED");
			retVal = true;
		}

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumCCMFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: CLOCK is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumDCDCFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: BROWN-OUT DETECTED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumWDOGFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: WATCHDOG is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumSDRAMFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: WEAK SDRAM DETECTED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumMPUFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: MPU is DISABLED or MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumNVICFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: INTERRUPT CONTROLLER SANITY FAILED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumI2CFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: I2Cn are MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumBEEFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: BEE is DISABLED or MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumADCFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 2 FAILED: ADC CALIBRATION FAILED");

		return retVal;
	}
	bool PostManager::RunPhase3InternalSysAcceltest (void)
	{
		bool retVal = false;
		uint32_t results = 0;

		results |= static_cast<uint32_t>(Mcal::Post::VerifyTrngEntropy());
		results |= static_cast<uint32_t>(Mcal::Post::TestEdmaTransfer());
		results |= static_cast<uint32_t>(Mcal::Post::VerifyDcpFunctional());
		results |= static_cast<uint32_t>(Mcal::Post::VerifyRtcTicking());
		results |= static_cast<uint32_t>(Mcal::Post::VerifyCacheStatus());
		results |= static_cast<uint32_t>(Mcal::Post::VerifyFlexIoClock());
		results |= static_cast<uint32_t>(Mcal::Post::VerifySecondaryFlash());
		results |= static_cast<uint32_t>(Mcal::Post::VerifyFlashRemap());
		results |= static_cast<uint32_t>(Mcal::Post::AuditLifetimeCounters());

		if(results == 0)
		{
			Service::Logger::Log(LogLevel::enumInfo, "[POST] Phase 3: Internal System accelerators PASSED");
			retVal = true;
		}

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumTRNGFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 3 FAILED: TRNG is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumDMAFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 3 FAILED: DMA is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumDCPFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 3 FAILED: DCP is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumSNVSFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 3 FAILED: SNVS is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumCacheFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 3 FAILED: Cache is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumFlexIOFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 3 FAILED: FlexIO is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumFlexSPI2Fail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 3 FAILED: FlexSPI is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumOTAFail))
		        Service::Logger::Log(LogLevel::enumError, "[POST] Phase 3 FAILED: OTA is MISCONFIGURED");

		if (results & static_cast<uint32_t>(Mcal::PostStatus::enumLifetimeWarning))
		        Service::Logger::Log(LogLevel::enumWarning, "[POST] Phase 3 WARNING: Need servicing");

		return retVal;
	}
	bool PostManager::RunPhase4ExternalFunctionalTest (void)
	{
		bool retVal = false;

		return retVal;
	}
}


