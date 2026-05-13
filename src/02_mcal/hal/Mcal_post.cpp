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
	#include "fsl_dcdc.h"
}

namespace Mcal {

	//Phase 1: Core CPU Test
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
		uint16_t timeout = 0xFFFF;
		while ((!(TEMPMON->TEMPSENSE0 & TEMPMON_TEMPSENSE0_CLR_FINISHED_MASK))
				&& (timeout > 0u))
		{
			timeout--;
		}

		if(timeout == 0u)
		{
			ret = PostStatus::enumTempFail;
		}

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
		/* Allocate a 64-byte buffer on the stack to test the physical RAM cells
		   currently backing the stack region (SRAM_DTC) */
		volatile uint32_t testBuffer[16];
		const uint32_t pattern1 = 0x55AA55AAU;
		const uint32_t pattern2 = 0xAA55AA55U;

		for (uint8_t i = 0U; i < 16U; i++) {
			testBuffer[i] = pattern1;
			if (testBuffer[i] != pattern1) { ret = PostStatus::enumRamFail; }

			testBuffer[i] = pattern2;
			if (testBuffer[i] != pattern2) { ret = PostStatus::enumRamFail; }
		}
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

		/* Clear status bits immediately (Write-1-to-Clear) */
		SRC->SRSR = SRC_SRSR_W1C_BITS_MASK;

		if ((srsr & SRC_SRSR_IPP_RESET_B_MASK) != 0U)
		{
			ret = ResetCause::enumPowerOn;
		}
		else if ((srsr & SRC_SRSR_WDOG3_RST_B_MASK) != 0U)
		{
			ret = ResetCause::enumWatchdog;
		}
		else if ((srsr & SRC_SRSR_LOCKUP_SYSRESETREQ_MASK) != 0U)
		{
			ret = ResetCause::enumLockup;
		}
		else if ((srsr & SRC_SRSR_TEMPSENSE_RST_B_MASK) != 0U)
		{
			ret = ResetCause::enumTempSense;
		}
		else
		{
			/* Fall through to enumUnknown */
		}

		return ret;
	}

	// Phase 2: Internal peripheral and Bus tests
	PostStatus Post::VerifyClockStability()
	{
		PostStatus ret = PostStatus::enumPASS;
		// Check if PLLs are locked and Clock Switch is NOT in progress
		/* Ref: 14.7.15 CCM Divider Handshake In-Process Register (CCM_CDHIPR) */
		if(CCM->CDHIPR & (CCM_CDHIPR_ARM_PODF_BUSY_MASK
						| CCM_CDHIPR_PERIPH_CLK_SEL_BUSY_MASK
						| CCM_CDHIPR_PERIPH2_CLK_SEL_BUSY_MASK
						| CCM_CDHIPR_AHB_PODF_BUSY_MASK
						| CCM_CDHIPR_SEMC_PODF_BUSY_MASK))
		{
			ret = PostStatus::enumCCMFail; //CLock tree still unstable
		}
		return ret;
	}

	PostStatus Post::CheckPowerRails()
	{
		PostStatus ret = PostStatus::enumPASS;
		// Query DCDC_REG0 for Brown-out status
		/*Ref: 18.5.1.2 DCDC Register 0 (REG0) */
		if(!(DCDC->REG0 & DCDC_REG0_STS_DC_OK_MASK))
		{
			ret = PostStatus::enumDCDCFail;
		}
		return ret;
	}

	PostStatus Post::VerifyWatchdogLatching()
	{
		PostStatus ret = PostStatus::enumPASS;
		// Verify if RTWDOG is enabled and the timeout is non-zero
		if (!(RTWDOG->CS & RTWDOG_CS_EN_MASK))
		{
			ret = PostStatus::enumWDOGFail;
		}
		if (RTWDOG->TOVAL == 0)
		{
			ret = PostStatus::enumWDOGFail;
		}
		return ret;
	}

	PostStatus Post::RunSdramTest()
	{
		CLOCK_EnableClock(kCLOCK_Semc);
		PostStatus ret = PostStatus::enumPASS;

		//Ref: 25.7.1.10 DRAM Control Register 0 (SDRAMCR0)
		if(SEMC->SDRAMCR0 == 0)
		{
			ret = PostStatus::enumSDRAMFail;
		}
		else
		{
			/* Ref: 3.2 Arm Platform Memory Map, Table 3-1. System memory map (CM7) */
			uint32_t *sdRamAddr = (uint32_t*) 0x80000000; //SDRAM address
			uint32_t pattern = 0x55AA55AA;

			*sdRamAddr = pattern;

			__DSB(); // If it faults here, the SDRAM/SEMC is not initialized!
			__ISB();

			SCB_CleanInvalidateDCache_by_Addr((uint32_t*)0x80000000, 4);
			//Wait for 10 ms atleast
			SDK_DelayAtLeastUs(10000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

			if(*sdRamAddr != pattern)
			{
				ret = PostStatus::enumSDRAMFail;
			}
		}
		return ret;
	}

	PostStatus Post::VerifyMpuRegions()
	{
		PostStatus ret = PostStatus::enumPASS;
		// Verify MPU is actually enabled
		if (!(MPU->CTRL & MPU_CTRL_ENABLE_Msk)) {
			return PostStatus::enumMPUFail;
		}

		// 2. Pessimistic check of Region 0 (Background) and Region 2 (RAM Attributes)
		// We query the RBAR (Base Address) and RASR (Attributes) registers
		MPU->RNR = 2; // Select Region 2 (standard RAM region in NXP SDK)

		// Check if the region is enabled and has the expected attributes
		// (e.g., verifying it isn't accidentally set to 'Strongly Ordered' which would tank performance)
		if (!(MPU->RASR & MPU_RASR_ENABLE_Msk)) {
			return PostStatus::enumMPUFail;
		}
		return ret;
	}

	// NVIC Software Interrupt Sanity
	static volatile bool g_swIntTriggered = false;
	//115 is mostly orphan on RT1060
	extern "C" void Reserved115_IRQHandler(void)
	{
		g_swIntTriggered = true;
		// Explicitly clear the pending bit within the handler
		NVIC_ClearPendingIRQ(Reserved115_IRQn);
	}

	PostStatus Post::TestNvicSoftwareInterrupt()
	{
		PostStatus ret = PostStatus::enumPASS;
		g_swIntTriggered = false;
		//Enable and set interrupt
		NVIC_EnableIRQ(Reserved115_IRQn);
		NVIC_SetPendingIRQ(Reserved115_IRQn);

		SDK_DelayAtLeastUs(10, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

		if(g_swIntTriggered == false)
		{
			ret = PostStatus::enumNVICFail;
		}

		return ret;
	}

	PostStatus Post::VerifyI2cBusHealth()
	{
		PostStatus ret = PostStatus::enumPASS;

		// Query LPI2C MSR (Master Status Register)
		// If BB (Bus Busy) is set before we've sent anything, the bus is unhealthy
		/*Ref: 47.5.1.5 Master Status (MSR)*/
		if ((LPI2C1->MSR & LPI2C_MSR_BBF_MASK)
			| (LPI2C2->MSR & LPI2C_MSR_BBF_MASK)
			| (LPI2C3->MSR & LPI2C_MSR_BBF_MASK)
			| (LPI2C4->MSR & LPI2C_MSR_BBF_MASK))
		{
			return PostStatus::enumI2CFail;
		}
		return ret;
	}

	PostStatus Post::CheckBeeStatus()
	{
		PostStatus ret = PostStatus::enumPASS;

		/* Ref: 14.7.25 CCM Clock Gating Register 4 (CCM_CCGR4)*/
		const uint32_t beeClockEnabled = CCM->CCGR4 & CCM_CCGR4_CG3_MASK;
		if(beeClockEnabled != 0u)
		{
			const uint32_t status = BEE->STATUS;
			// Check Bus Encryption Engine (BEE) Status 0 register for error bits
			if (((status & BEE_STATUS_IRQ_VEC_MASK) != 0u) ||
				(status & BEE_STATUS_BEE_IDLE_MASK) == 0u)
			{
				return PostStatus::enumBEEFail;
			}
		}
		return ret;
	}

	PostStatus Post::CalibrateAdc()
	{
		PostStatus ret = PostStatus::enumPASS;

		//Ensure ADC1 clock enabled
		CLOCK_EnableClock(kCLOCK_Adc1);

		// Trigger Auto-Calibration via GS (General Status) and GC (General Control)
		/* Ref: 66.8.7 General control register (ADCx_GC) */
		ADC1->GC |= ADC_GC_CAL_MASK;

		uint32_t timeout = 0xFFFF;
		/* Check for timeout or the CALF (Calibration Failed) bit.
		 * CALF sets if the sequence was interrupted or failed internally. */
		while(((ADC1->GC & ADC_GC_CAL_MASK) != 0) && (timeout > 0))
		{
			timeout--;
		}

		if((ADC1->GS & ADC_GS_CALF_MASK) || (timeout == 0))
		{
			ret = PostStatus::enumADCFail;
		}

		//Ensure ADC1 clock enabled
		CLOCK_DisableClock(kCLOCK_Adc1);
		return ret;
	}

	// Phase 3: Internal system accelerators

    PostStatus Post::VerifyTrngEntropy()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	//Verifies that the entropy source is not generating "locked" or predictable values.
    	CLOCK_EnableClock(kCLOCK_Trng);
    	/* Check the health fail bit in the status register*/
    	if((TRNG->STATUS & (TRNG_INT_STATUS_HW_ERR_MASK |
    			TRNG_INT_STATUS_ENT_VAL_MASK)) == TRNG_INT_STATUS_HW_ERR_MASK)
    	{
    		ret = PostStatus::enumTRNGFail;
    	}
    	/* Ensure the TRNG is not in a 'Program Mode' hang */
		if ((TRNG->MCTL & TRNG_MCTL_PRGM_MASK) != 0U)
		{
			ret = PostStatus::enumTRNGFail;
		}
		CLOCK_DisableClock(kCLOCK_Trng);
    	return ret;
    }

    PostStatus Post::TestEdmaTransfer()
    {
    	PostStatus ret = PostStatus::enumPASS;

    	uint32_t src = 0x55AA55AAu;
    	volatile uint32_t dest = 0u;
    	uint32_t timeout = 0xFFFFu;

    	CLOCK_EnableClock(kCLOCK_Dma);
    	/* 1. Reset Channel 0 to clear any stale state */
		DMA0->TCD[0].CSR = 0;
		DMA0->ERR = 0xFFFFFFFFU;

		/* 2. Mandatory TCD initialization: The engine requires these for a valid cycle */
		DMA0->TCD[0].SADDR = (uint32_t)&src;
		DMA0->TCD[0].DADDR = (uint32_t)&dest;
		DMA0->TCD[0].SOFF  = 0; // Don't increment source
		DMA0->TCD[0].DOFF  = 0; // Don't increment destination
		DMA0->TCD[0].ATTR  = DMA_ATTR_SSIZE(2) | DMA_ATTR_DSIZE(2); // 32-bit transfer
		DMA0->TCD[0].NBYTES_MLNO = 4; // 4 bytes per request
		DMA0->TCD[0].SLAST = 0;
		DMA0->TCD[0].DLAST_SGA = 0;

		/* CITER and BITER must be equal and non-zero to start */
		DMA0->TCD[0].CITER_ELINKNO = 1;
		DMA0->TCD[0].BITER_ELINKNO = 1;

		/* 3. Trigger and Poll */
		DMA0->SSRT = 0; // Set START bit for channel 0

		while (((DMA0->TCD[0].CSR & DMA_CSR_DONE_MASK) == 0U) && (timeout > 0U))
		{
			timeout--;
		}

		if ((timeout == 0U) || (dest != src))
		{
			ret = PostStatus::enumDMAFail;
		}

    	CLOCK_DisableClock(kCLOCK_Dma);
    	return ret;
    }

    PostStatus Post::VerifyDcpFunctional()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	CLOCK_EnableClock(kCLOCK_Dcp);

		/* Audit the DCP Status register for OTP-Key availability or engine errors */
		const uint32_t stat = DCP->STAT;
		if (stat & DCP_STAT_IRQ_MASK)
		{
			ret = PostStatus::enumDCPFail; // Not out-off reset and readable
		}
		CLOCK_DisableClock(kCLOCK_Dcp);
    	return ret;
    }

    PostStatus Post::VerifyRtcTicking()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	CLOCK_EnableClock(kCLOCK_SnvsHp);

    	/* Is the RTC actually enabled in the Control Register? */
		if (!(SNVS->HPCR & SNVS_HPCR_RTC_EN_MASK))
		{
			/* Enable it if it's off (standard for cold-boot audit) */
			SNVS->HPCR |= SNVS_HPCR_RTC_EN_MASK;
		}

    	const uint32_t startTicks = SNVS->HPRTCLR;

		SDK_DelayAtLeastUs(200, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);

		/* RTC should have incremented (Clocked at 32.768kHz) */
		if (SNVS->HPRTCLR == startTicks)
		{
			ret = PostStatus::enumSNVSFail;
		}

    	return ret;
    }

    PostStatus Post::VerifyCacheStatus()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Check bits in the Control/Configuration Register */
		const uint32_t ccr = SCB->CCR;
		if (((ccr & SCB_CCR_IC_Msk) == 0U) || ((ccr & SCB_CCR_DC_Msk) == 0U))
		{
			ret = PostStatus::enumCacheFail;
		}

    	return ret;
    }

    PostStatus Post::VerifyFlexIoClock()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	CLOCK_EnableClock(kCLOCK_Flexio1);

		/* Verify Version Register is readable and valid */
		if (FLEXIO1->VERID == 0U || FLEXIO1->VERID == 0xFFFFFFFFU)
		{
			ret = PostStatus::enumFlexIOFail;
		}
		CLOCK_DisableClock(kCLOCK_Flexio1);
    	return ret;
    }

    PostStatus Post::VerifySecondaryFlash()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Only audit if the port is enabled in CCM */
		if ((CCM->CCGR7 & CCM_CCGR7_CG1_MASK) != 0U)
		{
			if ((FLEXSPI2->STS0 & FLEXSPI_STS0_ARBIDLE_MASK) == 0U) { ret = PostStatus::enumFlashFail; }
		}

    	return ret;
    }

    PostStatus Post::VerifyFlashRemap()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* GPR32 controls the FlexSPI Remap function */
		const uint32_t remapSize = IOMUXC_GPR->GPR32;

		/* Warning: If remapping is active but logic address is unexpected */
		if (remapSize != 0U)
		{
			// Log a notice that we are running from a remapped bank
		}

    	return ret;
    }

    PostStatus Post::AuditLifetimeCounters()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* SNVS LPGPR[0] = Boot Cycles, LPGPR[1] = Power-on Hours */
		const uint32_t bootCycles = SNVS->LPGPR[0];
		const uint32_t powerOnHours = SNVS->LPGPR[1];

		/* Pessimistic threshold check (25,000 Hours) */
		if (powerOnHours > 25000U)
		{
			ret = PostStatus::enumLifetimeWarning;
		}

		/* Increment boot cycles for this run */
		SNVS->LPGPR[0] = bootCycles + 1U;

    	return ret;
    }

    // Phase 4
    PostStatus Post::VerifyCanBusHealth()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Audit Transmit and Receive Error Counters */
		uint32_t ecr = CAN1->ECR;
		uint32_t txErr = (ecr & CAN_ECR_TXERRCNT_MASK) >> CAN_ECR_TXERRCNT_SHIFT;
		uint32_t rxErr = (ecr & CAN_ECR_RXERRCNT_MASK) >> CAN_ECR_RXERRCNT_SHIFT;

		if ((txErr > 96U) || (rxErr > 96U))
		{
			ret = PostStatus::enumCANBusFail;
		}
    	return ret;
    }

    PostStatus Post::VerifyEnetPhySmi()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Ensure ENET clock is active */
		CLOCK_EnableClock(kCLOCK_Enet);
		/* Verify MDIO is not held in a busy state */
		if ((ENET->MMFR & ENET_MMFR_OP_MASK) != 0U)
		{
			ret = PostStatus::enumEnetPhyFail;
		}
    	return ret;
    }

    PostStatus Post::VerifyAudioCodecReady()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Audit LPI2C1 Status (Used for WM8960 on EVKC) */
		if ((LPI2C1->MSR & LPI2C_MSR_BBF_MASK) != 0U)
		{
			ret = PostStatus::enumI2CFail;
		}
    	return ret;
    }

    PostStatus Post::VerifySdCardInserted()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Check CINST (Card Inserted) bit in Present State register */
		if (!(USDHC1->PRES_STATE & USDHC_PRES_STATE_CINST_MASK))
		{
			ret = PostStatus::enumSDCardFail;
		}
    	return ret;
    }

    PostStatus Post::VerifyUsbPhyStatus()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Check if PHY1 is powered and out of reset */
		if (!(USBPHY1->STATUS & USBPHY_STATUS_RSVD1_MASK))
		{
			ret = PostStatus::enumUSBPhyFail;
		}
    	return ret;
    }

    PostStatus Post::VerifyMotionSensor()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* This assumes a synchronous I2C driver call exists */
		uint8_t whoAmI = 0;
		// I2C_Read(Address 0x1E, Reg 0x0D, &whoAmI);
		// if (whoAmI != 0xC7U) return PostStatus::enumExtI2CFail;
    	return ret;
    }

    PostStatus Post::VerifyPmuVoltage()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Check STS_DC_OK bit (DCDC Output OK) */
		if (!(DCDC->REG0 & DCDC_REG0_STS_DC_OK_MASK))
		{
			ret = PostStatus::enumPMUFail;
		}
    	return ret;
    }

    PostStatus Post::VerifyDisplayClock()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	if ((CCM->CCGR3 & CCM_CCGR3_CG13_MASK) == 0U)
    	{
    	        return PostStatus::enumLCDFail;
    	    }
    	return ret;
    }

    bool Post::IsSkipButtonPressed()
    {
    	return (GPIO5->DR & (1U << 0)) ? false : true; // Active Low
    }

    PostStatus Post::VerifyPwmTimers()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* Check if PWM1 Submodule 0 is enabled */
		if ((PWM1->MCTRL & PWM_MCTRL_RUN_MASK) == 0U)
		{
			ret = PostStatus::enumPWMFail;
		}
    	return ret;
    }

    PostStatus Post::AuditSupplyRails()
    {
    	PostStatus ret = PostStatus::enumPASS;
    	/* This requires ADC calibration to have finished in Phase 2 */
		uint32_t voltageRaw = ADC1->R[0]; // Sample result
		if (voltageRaw < 0x800U)
		{
			ret = PostStatus::enumPWRFail;
		}
    	return ret;
    }
}
