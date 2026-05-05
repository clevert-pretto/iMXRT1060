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
		enumRevisionFail= (1 << 5),
		enumCCMFail		= (1 << 6),
		enumDCDCFail	= (1 << 7),
		enumWDOGFail	= (1 << 8),
		enumSDRAMFail	= (1 << 9),
		enumMPUFail		= (1 << 10),
		enumNVICFail	= (1 << 11),
		enumI2CFail		= (1 << 12),
		enumBEEFail		= (1 << 13),
		enumADCFail		= (1 << 14),
		enumTRNGFail	= (1 << 15),
	    enumDMAFail		= (1 << 16),
	    enumDCPFail		= (1 << 17),
	    enumSNVSFail	= (1 << 18),
	    enumCacheFail	= (1 << 19),
	    enumFlexIOFail	= (1 << 20),
	    enumFlexSPI2Fail= (1 << 21),
	    enumOTAFail		= (1 << 22),
	    enumLifetimeWarning	= (1 << 23) // Non-fatal advisory warning

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

		// Phase 2: Internal Peripheral and Bus tests
		static PostStatus VerifyClockStability();		// Test 1 & 2
		static PostStatus CheckPowerRails();          	// Test 3
        static PostStatus VerifyWatchdogLatching();    	// Test 4
        static PostStatus RunSdramTest();             	// Test 5
        static PostStatus VerifyMpuRegions();         	// Test 6
        static PostStatus TestNvicSoftwareInterrupt(); 	// Test 7
        static PostStatus VerifyI2cBusHealth();       	// Test 8
        static PostStatus CheckBeeStatus();           	// Test 9
        static PostStatus CalibrateAdc();             	// Test 10

        // Phase 3: Internal system accelerators
        static PostStatus VerifyTrngEntropy();			//Test 1
        static PostStatus TestEdmaTransfer();			//Test 2
        static PostStatus VerifyDcpFunctional();		//Test 3
        static PostStatus VerifyRtcTicking();			//Test 4
        static PostStatus VerifyCacheStatus();			//Test 5
        static PostStatus VerifyFlexIoClock();			//Test 6
        static PostStatus VerifySecondaryFlash();		//Test 7
        static PostStatus VerifyFlashRemap();			//Test 8
        static PostStatus AuditLifetimeCounters();		//Test 9

	};
}







#endif /* __MCAL_POST_HPP_ */
