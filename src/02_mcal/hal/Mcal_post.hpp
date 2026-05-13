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
	enum class PostStatus : uint64_t
	{
		//Phase 1
		enumPASS = 0,
		enumCpuFail     = (1ull << 0u),
		enumTempFail    = (1ull << 1u),
		enumFlexRamFail = (1ull << 2u),
		enumRamFail     = (1ull << 3u),
		enumFlashFail   = (1ull << 4u),
		enumRevisionFail= (1ull << 5u),
		//Phase 2
		enumCCMFail		= (1ull << 6u),
		enumDCDCFail	= (1ull << 7u),
		enumWDOGFail	= (1ull << 8u),
		enumSDRAMFail	= (1ull << 9u),
		enumMPUFail		= (1ull << 10u),
		enumNVICFail	= (1ull << 11u),
		enumI2CFail		= (1ull << 12u),
		enumBEEFail		= (1ull << 13u),
		enumADCFail		= (1ull << 14u),
		//Phase 3
		enumTRNGFail	= (1ull << 15u),
	    enumDMAFail		= (1ull << 16u),
	    enumDCPFail		= (1ull << 17u),
	    enumSNVSFail	= (1ull << 18u),
	    enumCacheFail	= (1ull << 19u),
	    enumFlexIOFail	= (1ull << 20u),
	    enumFlexSPI2Fail= (1ull << 21u),
	    enumOTAFail		= (1ull << 22u),
	    enumLifetimeWarning	= (1ull << 23u), // Non-fatal advisory warning
		//Phase 4
		enumCANBusFail	= (1ull << 24u),
		enumEnetPhyFail	= (1ull << 25u),
		enumCodecFail	= (1ull << 26u),
		enumSDCardFail	= (1ull << 27u),
		enumUSBPhyFail	= (1ull << 28u),
		enumExtI2CFail	= (1ull << 29u),
		enumPMUFail	= (1ull << 30u),
		enumLCDFail	= (1ull << 31u),
		enumPWMFail	= (1ull << 32u),
		enumPWRFail	= (1ull << 33u),

		enumMAXFail = (1ull << 63u)
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

        //Phase 4: External Functional test
        /* Verifies if the CAN engine has encountered bus errors
         * (e.g., bit stuffing or form errors) during initialization.
         */
        static PostStatus VerifyCanBusHealth();			//Test 1

        /*Queries the external PHY chip via the SMI
         * (Serial Management Interface) to verify connectivity.*/
        static PostStatus VerifyEnetPhySmi();			//Test 2 & 3

        /* Uses the I2C bus to ping the audio codec. If the bus hangs,
         *  it indicates a hardware pull-up failure or a dead codec.*/
        static PostStatus VerifyAudioCodecReady();		//Test 4

        /* Checks the physical insertion bit from the SD card controller. */
        static PostStatus VerifySdCardInserted();		//Test 5

        /* Verifies the internal USB PHYs have stabilized their power-on state. */
        static PostStatus VerifyUsbPhyStatus();			//Test 6

        /* Performs a "Who Am I" auditon the FXOS8700 sensor*/
        static PostStatus VerifyMotionSensor();			//Test 7

        /* Ensures the internal regulator is providing the 1.1V required for the 600MHz CPU overdrive */
        static PostStatus VerifyPmuVoltage();			//Test 8

        /* Verifies the display interface clock is running before driving the panel. */
        static PostStatus VerifyDisplayClock();			//Test 9

        /* Allows the POST to be bypassed or interactive by checking the GPIO state */
        static bool IsSkipButtonPressed();		//Test 10

        /* Verifies the FlexPWM submodule clocking is active */
        static PostStatus VerifyPwmTimers();			//Test 11

        /* Measures the 3.3V rail (via a voltage divider) to ensure board power health */
        static PostStatus AuditSupplyRails();			//Test 12
        //Test 13 'stack high-watermark' will be part of osal_rtos
	};
}







#endif /* __MCAL_POST_HPP_ */
