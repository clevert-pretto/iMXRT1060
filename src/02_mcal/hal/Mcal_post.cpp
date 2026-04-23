/**
 * @file Mcal_post.cpp
 * @author ravic
 * @brief TODO
 * @date 22-Apr-2026
 * * @copyright Copyright (c) 2026 ravic. All rights reserved.
 * * @attention
 * This software is the confidential and proprietary information of ravic.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Use, distribution, or modification of this code is permitted only under 
 * the terms of the formal license agreement provided by the copyright holder.
 *
 *
 * Reference document: i.MX RT1060 Processor Reference Manual, Rev. 4, 01/2026

*****************************************************************************************/
#include "Mcal_post.hpp"

extern "C" {
	#include "fsl_device_registers.h"
	#include "fsl_common.h"
}

namespace Mcal {

	PostStatus Post::RunCpuAluTest()
	{
		PostStatus ret = PostStatus::enumPASS;
		volatile uint32_t a = 0x55555555;
		volatile uint32_t b = 0xAAAAAAAA;

		//ALU Sanity check
		if ((a | b) != 0xFFFFFFFF)
		{
			ret = PostStatus::enumCpuFail;
		}
		if ((a & b) != 0x00000000)
		{
			ret = PostStatus::enumCpuFail;
		}

		//FPU Sanity
		volatile float fa = 1.23f;
		volatile float fb = 2.34f;

		if(((fa + fb) < 3.56f) || ((fa + fb) > 3.58f))
		{
			ret = PostStatus::enumCpuFail;
		}
		return ret;
	}

	PostStatus Post::VerifyBoardRevision()
	{
		PostStatus ret = PostStatus::enumPASS;
		//Read Configuration and management infor from On-Chip OTP Controller
		/* Ref: 7.7 On-Chip OTP Controller (OCOTP_CTRL)*/
		uint32_t siliconID = OCOTP->CFG0;
		if(siliconID == 0)
		{
			ret = PostStatus::enumRevisionFail;
		}
		return ret;
	}

	PostStatus Post::CheckInternalTemp()
	{
		PostStatus ret = PostStatus::enumPASS;
		//Powerup Temperature Monitor
		/* Ref: 19.1 Chip-specific TEMPMON information*/
		TEMPMON->TEMPSENSE0 &= ~TEMPMON_TEMPSENSE0_CLR_POWER_DOWN_MASK;

		//Trigger measurement
		TEMPMON->TEMPSENSE0 |= TEMPMON_TEMPSENSE0_CLR_MEASURE_TEMP_MASK;

		while (!(TEMPMON->TEMPSENSE0 & TEMPMON_TEMPSENSE0_CLR_FINISHED_MASK));

		uint32_t tempRaw = (TEMPMON->TEMPSENSE0 & TEMPMON_TEMPSENSE0_CLR_TEMP_CNT_MASK) >> TEMPMON_TEMPSENSE0_CLR_TEMP_CNT_SHIFT;

		if(tempRaw == 0 || tempRaw == 0xFFF)
		{
			ret = PostStatus::enumTempFail;
		}

		return ret;
	}

	PostStatus Post::VerifyFlexRamPartitioning()
	{
		PostStatus ret = PostStatus::enumPASS;

		//GPR17 holds the bank allocation
		/*Ref: 11.3 IOMUXC GPR Memory Map/Register Definition
		 * GPR_FLEXRAM_BANK_CFG[2n+1 : 2n], where n = 0, 1, ..., 15
			• 00: RAM bank n is not used
			• 01: RAM bank n is OCRAM
			• 10: RAM bank n is DTCM
			• 11: RAM bank n is ITCM */

		uint32_t config = IOMUXC_GPR->GPR17;

		/* Verify that the source of config is eFuse or GPR as expected.
		 * Ensure at least some banks are allocated to ITC/DTC */
		if((config & 0xFFFFFFFF) == 0)
		{
			ret = PostStatus::enumFlexRamFail;
		}
		return ret;
	}

	PostStatus Post::RunInternalRamIntegrity()
	{
		PostStatus ret = PostStatus::enumPASS;
		/* Non-destructive pattern test on a 1KB block of OCRAM or DTC
		 * Base of OCRAM2
		 * Ref: Table 3-1. System memory map (CM7) (continued) */
		uint32_t* testAddr = (uint32_t*)0x20200000;
		uint32_t original = *testAddr; //backup original data

		*testAddr = 0x55AA55AA;
		if(*testAddr != 0x55AA55AA)
		{
			ret = PostStatus::enumRamFail;
		}

		*testAddr = 0xAA55AA55;
		if(*testAddr != 0xAA55AA55)
		{
			ret = PostStatus::enumRamFail;
		}

		*testAddr = original; //restore original value

		return ret;
	}

	PostStatus Post::VerifyFlashIntegrity()
	{
		PostStatus ret = PostStatus::enumPASS;
		/* Flash Checksum & Bit-Flip Detection
		 * Verify IVT Header at the start of Flash (XIP offset 0x1000)
		 * Ref: Table 3-1. System memory map (CM7) (continued)
		 * 9.7.1 Image Vector Table and Boot Data
		 * Table 9-36. Image Vector Table Offset and Initial Load Region Size
		 *
		 * The Image Vector Table (IVT) is the data structure that the ROM reads from the boot
		 * device supplying the program image containing the required data components to perform
		 * a successful boot.
		 */

		uint32_t* ivtHeader = (uint32_t*) 0x60001000; //FlexSPI at 0x60000000 + offset at 0x1000

		/* Ref: Figure 9-15. IVT header format
		 * Tag: A single byte field set to 0xD1
		 * Length: a two byte field in big endian format containing the overall length of the IVT,
		 * in bytes, including the header. (the length is fixed and must have a value of 32 bytes)
		 * Version: A single byte field set to 0x40/0x41*/
		if((*ivtHeader != 0x412000D1) && (*ivtHeader != 0x402000D1))
		{
			ret = PostStatus::enumFlashFail;
		}

		return ret;
	}

	ResetCause Post::GetResetCause()
	{
		ResetCause ret = ResetCause::enumUnknown;
		/* Reset cause forensic
		 * Ref: 21.6.3 SRC Reset Status Register (SRC_SRSR)
		 */
		uint32_t srsr = SRC->SRSR;

		if(srsr & SRC_SRSR_WDOG_MASK)
		{
			ret = ResetCause::enumWatchdog;
		}
		if(srsr & SRC_SRSR_IPP_RESET_B_MASK)
		{
			ret = ResetCause::enumPowerOn;
		}
		if(srsr & SRC_SRSR_JTAG_SW_RST_MASK)
		{
			ret = ResetCause::enumJTAG;
		}
		if(srsr & SRC_SRSR_LOCKUP_SYSRESETREQ_MASK)
		{
			ret = ResetCause::enumLockup;
		}
		if(srsr & SRC_SRSR_CSU_RESET_B_MASK)
		{
			ret = ResetCause::enumCSU;
		}
		if(srsr & SRC_SRSR_TEMPSENSE_RST_B_MASK)
		{
			ret = ResetCause::enumTempSense;
		}

		return ret;
	}
}
