---
layout: default
title: "Power On Self Test - Trust no one"
nav_order: 4
has_children: true
---

This Power-On Self-Test (POST) architecture for the MIMXRT1060-EVKC follows an "Inward-to-Outward" design pattern. It begins at the silicon core to establish a "Root of Trust" in the hardware and moves progressively through internal buses to external board-level integrated circuits (ICs).

# "Root of Trust" POST Architecture
|-Phase-|-Reliability Focus-|-Pessimistic Metric-|
|-------|-------------------|--------------------|
|1 (Core)|Integrity|Reset Cause != Watchdog; FlexRAM matches Linker.|
|2 (Bus)|Stability|DCDC Brown-out bits == 0; PLL Locked bit == 1.|
|3 (Accel)|Security|"TRNG Health == OK; HAB Status == ""Closed"" (in Prod)."|
|4 (Board)|Environment|Rails ±5%; Task Stacks <80% usage.|

--------------------------------------------------------------------------------------------
# Phase 1: Critical Core Tests (Pre-OS / Bare-Metal)

- These tests must run in the ResetISR or at the very beginning of main() before `__libc_init_array()` and before the `OSAL` is initialized. 

- These verify the silicon and primary memory paths.

## 1. CPU Register & ALU Test: 

Verify basic CPU functionality and the Floating Point Unit (FPU).

## 2. Board Revision/ID Verification:
Query a "unique silicon ID" or "Board ID" to verify that the software configuration matches the physical board revision.

## 3. Internal Temperature Sensor (TEMPMON):

Verify the silicon die temperature is within the industrial or commercial range. 

If the board is in an extreme environment or the heatsink is loose, we should catch this before the high-performance clocks are engaged.

## 4. FlexRAM Partitioning Verification:

Since we are testing the `128KB` DTC/ITC boundaries, we should first query the `IOMUXC_GPR` registers to verify that the FlexRAM is actually partitioned as expected. 

If the fuses or boot headers changed the partitioning, your RAM tests will trigger a hard fault.

## 5. Internal RAM Integrity (SRAM_DTC/ITC):

Perform a **non-destructive** "Walking 1s/0s" test on a small block of RAM not currently used by the stack.

Verify the memory map boundaries for the *128KB* DTC and *128KB* ITC regions.

## 6. FlexRAM Configuration Guard:

Register `IOMUXC_GPR->GPR17`: Verify that the FlexRAM bank allocation (`ITCM` vs `DTCM` vs `OCRAM`) matches your linker script. If a bit-flip occurs in the `eFuse` or `GPR` registers, your code will attempt to execute from non-existent memory, causing a `HardFault`.

## 7. Flash Checksum (XIP Verification):

Calculate a CRC32 or SHA checksum of the application code residing in `BOARD_FLASH (8MB)` and compare it against a stored value in the boot_hdr.

## 8. Bit-Flip Detection in Flash:

Instead of a simple CRC, implement a Header-First Integrity Check. Verify the `IVT` (Image Vector Table) and Boot Data structures. If these are corrupted, the ROM bootloader might have used fallback settings that differ from your application's expectations.

## 9. Reset Cause Forensic:

Check `SRC->SRSR`: Do not just assume a power-on, differentiate between a **cold boot, a Watchdog Reset, a JTAG Reset, or a Lockup Reset**. If the last reset was a Watchdog failure, Phase 1 should trigger a "Safe Mode" boot.

--------------------------------------------------------------------------------------------
# Phase 2: Internal Peripheral & Bus Tests (Post-MCAL Init)
Run these after `Mcal::Board::InitHardware()` but before starting the OS kernel. Use  `Service::Logger` to report status.

## 1. Clock Tree Verification: 

Query the `CCM (Clock Control Module)` to verify that the CPU is actually running at `600MHz` and the peripheral clocks are within expected ranges.

## 2. Clock Glitch & PLL Lock Monitor:

Check `CCM->CDHIPR`: Verify that the handoff from the `24MHz` RC oscillator to the `600MHz` PLL happened cleanly. A pessimistic system should check if the "Clock Switch in Progress" bits ever timed out, indicating a marginal crystal or PLL stability issue.

## 3. DCDC Brown-out Status:

Check `DCDC->REG0`: Query the DCDC for "Brown-out" status bits. If the core voltage dropped below the `1.25V` threshold during the high-current spike of the `600MHz` clock engagement, the system is at risk of silent data corruption.

## 4. Watchdog (RTWDOG):

Verify the watchdog can be serviced and that its timeout value is correctly latched.

## 5. External SDRAM (32MB BOARD_SDRAM):

Initialize the SEMC *(Smart External Memory Controller)*.

Perform a data bus and address bus wiring test (checking for shorts/opens) on the SDRAM.

Add a Data Retention Check. Write a pattern, wait `10ms`, then read it back. This catches "weak" SDRAM cells that pass immediate tests but fail under real-world refresh cycles.

## 6. MPU Configuration Check: 

Verify that memory protection regions for Cache and Non-Cacheable memory (`NCACHE_REGION`) are active.

## 7. NVIC/Interrupt Controller Sanity:

Manually trigger a "Software Interrupt" and verify the ISR executes before starting the OSAL.

## 8. Internal I2C/SPI Loopback or query ID:

Initialize LPI2C and LPSPI.

Before I2C initialization, check if SDA/SCL are high. If SDA is low, toggle SCL manually until the slave releases the bus.

If hardware permits, perform a loopback test or query the ID register of an on-board component (like the **FXOS8700 Accelerometer** via I2C) to verify bus health.

## 9. BEE (Bus Encryption Engine): 

If you eventually use the encrypted XIP features of the RT1060, adding a check to see if the BEE is enabled and its status is "Idle" (not in error) is a key security POST

## 10.ADC Calibration: 

The RT1060's ADCs are highly sensitive to power-up noise. Running the ADC Self-Calibration routine during Phase 2 ensures that any future sensor readings (like battery voltage or thermal) are accurate.

--------------------------------------------------------------------------------------------
# Phase 3: Internal System accelerators
Verify that autonomous silicon blocks are functional before starting the RTOS

## 1. TRNG (True Random Number Generator): 

Verify that the TRNG can generate entropy. This is critical for generating unique keys for MbedTLS or AWS IoT.

check for Health Fail bits in the `TRNG0->STA` register.

## 2. eDMA (Enhanced Direct Memory Access): 

Perform a simple memory-to-memory DMA transfer between SRAM_DTC and SRAM_OC. If this fails,  high-speed logging or audio processing will stall the CPU.

## 3. DCP (Data Co-Processor): 

Perform a simple hardware-accelerated AES or SHA test to ensure the crypto engine is functional.

## 4. SNVS (Secure Non-Volatile Storage) / RTC: 

Verify that the internal RTC is ticking and that the SNVS battery-backed domain is accessible.

## 5. Cache Health: 

The RT1060 has L1 Instruction and Data caches. While rare to fail, a POST could verify that the `L1CACHE_EnableICache()` and `L1CACHE_EnableDCache()` calls succeed without triggering a bus error.

## 6. FlexIO Check: 

If you use FlexIO to emulate protocols (like a camera interface or high-speed serial), verify the FlexIO clock and shifter status.

## 7. FlexSPI2 / Secondary Flash: 

If your board uses a second Flash chip (often present on the EVKC for data storage), verify its JEDEC ID via the second FlexSPI port.

## 8. Flash Aging Monitor (Remapping):

Check `IOMUXC_GPR->GPR32`: Use the RT1060's *hardware remapping feature* to verify that the active OTA (Over-the-Air) bank is the correct one. A pessimistic check ensures that a failed OTA didn't leave the system in a "Half-Boot" state.

## 9. Lifetime Counters (The "Flight Recorder"):

Store in `SNVS LP-GPR`: Use the battery-backed registers to track Total Boot Cycles and Power-On Hours (PoH).

Pessimistic Logic: If the system has reached >25,000 hours (the estimated product lifetime for certain high-voltage cases), trigger a "Service Required" warning via the Logger.
--------------------------------------------------------------------------------------------
# Phase 4: External Functional Tests (Post-OS Start)

Once the `OSAL` and `Service Layer` are running, start a dedicated **"POST Manager"** task to verify complex interfaces.

## 1. CAN-FD Controller: 

Query the CAN controller's error counters or perform a "Loopback-Internal" test (which doesn't require external wiring) to verify the CAN engine's timing.

## 2. Ethernet (ENET) PHY Link: 

Query the **PHY** via **MDIO** to verify a hardware link is established with the external connector.

## 3. Ethernet MDIO Speed: 

verify the MDIO communication speed. This confirms the SMI (**Serial Management Interface**) is stable before the OS attempts to negotiate a `100Mbps` link.

## 4. Audio Codec (SAI/WM8960): 

Perform an I2C communication test with the **WM8960** codec to ensure it responds and is ready for audio streams. This verifies that the I2C bus isn't hanging in a 'busy' state due to noise. 

## 5. MicroSD Card (USDHC): 

Verify the SD card slot by attempting to read the `CID/CSD` registers of an inserted card.

## 6. USB Controller: 

Check the status of the USB OTG controllers to ensure the PHYs are powered and initialized.


## 7. FXOS8700CQ (Accelerometer/Magnetometer): 

Query the "Who Am I" register via LPI2C. This verifies both the I2C bus health and the presence of the board's primary motion sensor.

## 8. DCDC/PMU Status: 

Query the Power Management Unit to ensure the internal DCDC converter is operating at the correct voltage levels for the `600MHz` core frequency.

## 9. LCDIF (LCD Interface): 

If a display is connected, verify that the pixel clock is active.

## 10. User Button: 

Since we previously worked on external interrupts, include a "press-to-skip" or "press-to-verify" check for the onboard buttons during the POST window.

## 11. PWM Channels: 

Briefly initialize a PWM channel (e.g., for an LED or buzzer) to verify the timer subsystems are functional.

## 12. Supply Rail ADC Audit:

Use the ADC to measure the actual voltage on the 3.3V and 1.8V rails. If a rail is at 3.0V, the system might boot but the Ethernet PHY or SD Card will fail intermittently.

## 13. Stack High-Water Marking:

In your OSAL-based tasks, the POST Manager should verify Stack Canaries. If task has used >90 of its stack size, the system is "Unsafe" even if it currently works.


--------------------------------------------------------------------------------------------
# POST Architectural Integration Plan

To keep this "Hardware and OS Agnostic," follow this implementation pattern:

## MCAL Layer (The "How"): 

Add a `Post_Drivers` class in src/02_mcal/. This class provides raw test functions (e.g., `Mcal::Post::TestRam()`, `Mcal::Post::VerifyClock()`).

## Service Layer (The "Management"): 

Create a `PostManager service`.

It defines an enum class `PostResult`.

It calls the MCAL tests and uses `Service::Logger` to log the results (e.g., `Log(LogLevel::enumInfo, "SDRAM TEST: PASSED")`).

## Application Layer (The "Policy"): 

In application logic, decide the system's fate based on the POST results.

--------------------------------------------------------------------------------------------
# Failure Handling Policy

## Critical Failure: 

Halt execution and blink an onboard LED (`GPIO1_IO09` on the EVKC) in a specific pattern.

## Non-Critical Failure: 

(e.g., `SD Card missing`) Log the warning and continue system start, but disable features dependent on that peripheral.


This architecture is robust because it doesn't just ask "Does the hardware work?", but also "Is the hardware environment safe for the software to survive?".

--------------------------------------------------------------------------------------------