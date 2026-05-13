# CROSS COMPILER SETTING
SET(CMAKE_SYSTEM_NAME Generic)
CMAKE_MINIMUM_REQUIRED (VERSION 3.10.0)

# THE VERSION NUMBER
SET (Tutorial_VERSION_MAJOR 1)
SET (Tutorial_VERSION_MINOR 0)

if (DEFINED ENV{SdkRootDirPath})
	SET(SdkRootDirPath "$ENV{SdkRootDirPath}")
endif()

if (NOT DEFINED SdkRootDirPath)
	MESSAGE(FATAL_ERROR "SDK path could not be determined. Please update SdkRootDirPath value from your CMake presets file(s).")
endif()

# ENABLE ASM
ENABLE_LANGUAGE(ASM)

# RESET FLAG VALUES
SET(CMAKE_ASM_FLAGS_DEBUG "")
SET(CMAKE_ASM_FLAGS_RELEASE "")
SET(CMAKE_C_FLAGS_DEBUG "")
SET(CMAKE_C_FLAGS_RELEASE "")
SET(CMAKE_CXX_FLAGS_DEBUG "")
SET(CMAKE_CXX_FLAGS_RELEASE "")
SET(CMAKE_EXE_LINKER_FLAGS_DEBUG "")
SET(CMAKE_EXE_LINKER_FLAGS_RELEASE "")

SET(CMAKE_STATIC_LIBRARY_PREFIX)
SET(CMAKE_STATIC_LIBRARY_SUFFIX)

SET(CMAKE_EXECUTABLE_LIBRARY_PREFIX)
SET(CMAKE_EXECUTABLE_LIBRARY_SUFFIX)

# CURRENT DIRECTORY
SET(ProjDirPath ${CMAKE_CURRENT_SOURCE_DIR})

SET(EXECUTABLE_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})
SET(LIBRARY_OUTPUT_PATH ${ProjDirPath}/${CMAKE_BUILD_TYPE})

set(CMAKE_USER_MAKE_RULES_OVERRIDE "user_override.cmake")

project(iMXRT1060)
set(MCUX_SDK_PROJECT_NAME iMXRT1060.elf)

include(${ProjDirPath}/flags.cmake)

add_executable(${MCUX_SDK_PROJECT_NAME} "${ProjDirPath}/src/05_app/sysConfig.hpp"
"${ProjDirPath}/src/05_app/main.cpp"
"${ProjDirPath}/src/05_app/appTask.hpp"
"${ProjDirPath}/src/05_app/appTask.cpp"
"${ProjDirPath}/src/04_services/svl_postManager.hpp"
"${ProjDirPath}/src/04_services/svl_postManager.cpp"
"${ProjDirPath}/src/04_services/Svl_logger.hpp"
"${ProjDirPath}/src/04_services/Svl_logger.cpp"
"${ProjDirPath}/src/04_services/svl_healthMonitor.hpp"
"${ProjDirPath}/src/04_services/svl_healthMonitor.cpp"
"${ProjDirPath}/src/03_os/Osal_rtos.hpp"
"${ProjDirPath}/src/03_os/Osal_freertos.cpp"
"${ProjDirPath}/src/03_os/FreeRTOSConfig.h"
"${ProjDirPath}/src/04_services/utilities/fsl_memcpy.S"
"${ProjDirPath}/src/04_services/utilities/fsl_assert.h"
"${ProjDirPath}/src/04_services/utilities/fsl_assert.c"
"${ProjDirPath}/src/04_services/diagnostics/semihost_hardfault.c"
"${ProjDirPath}/src/04_services/config/mcux_config.h"
"${ProjDirPath}/src/04_services/config/mcuxsdk_version.h"
"${ProjDirPath}/src/02_mcal/hal/Mcal_Watchdog.hpp"
"${ProjDirPath}/src/02_mcal/hal/Mcal_Watchdog.cpp"
"${ProjDirPath}/src/02_mcal/hal/Mcal_post.hpp"
"${ProjDirPath}/src/02_mcal/hal/Mcal_post.cpp"
"${ProjDirPath}/src/02_mcal/hal/Mcal_board.hpp"
"${ProjDirPath}/src/02_mcal/hal/Mcal_board.cpp"
"${ProjDirPath}/src/02_mcal/board/pin_mux.h"
"${ProjDirPath}/src/02_mcal/board/pin_mux.c"
"${ProjDirPath}/src/02_mcal/board/clock_config.h"
"${ProjDirPath}/src/02_mcal/board/clock_config.c"
"${ProjDirPath}/src/02_mcal/board/board.h"
"${ProjDirPath}/src/02_mcal/board/board.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_src.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_src.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_rtwdog.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_rtwdog.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_pmu.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_pmu.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_lpuart.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_lpuart.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_iomuxc.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_gpio.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_gpio.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_gpc.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_gpc.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_dcdc.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_dcdc.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_common_arm.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_common_arm.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_common.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_common.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_clock.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_clock.c"
"${ProjDirPath}/src/02_mcal/drivers/fsl_cache.h"
"${ProjDirPath}/src/02_mcal/drivers/fsl_cache.c"
"${ProjDirPath}/src/01_hw_platform/xip/fsl_flexspi_nor_boot.h"
"${ProjDirPath}/src/01_hw_platform/xip/fsl_flexspi_nor_boot.c"
"${ProjDirPath}/src/01_hw_platform/xip/evkcmimxrt1060_flexspi_nor_config.h"
"${ProjDirPath}/src/01_hw_platform/xip/evkcmimxrt1060_flexspi_nor_config.c"
"${ProjDirPath}/src/01_hw_platform/xip/dcd.h"
"${ProjDirPath}/src/01_hw_platform/xip/dcd.c"
"${ProjDirPath}/src/01_hw_platform/startup/startup_mimxrt1062.cpp"
"${ProjDirPath}/src/01_hw_platform/device/system_MIMXRT1062.h"
"${ProjDirPath}/src/01_hw_platform/device/system_MIMXRT1062.c"
"${ProjDirPath}/src/01_hw_platform/device/MIMXRT1062_features.h"
"${ProjDirPath}/src/01_hw_platform/device/MIMXRT1062_COMMON.h"
"${ProjDirPath}/src/01_hw_platform/device/MIMXRT1062.h"
"${ProjDirPath}/src/01_hw_platform/device/fsl_device_registers.h"
"${ProjDirPath}/src/01_hw_platform/CMSIS/core_cm7.h"
"${ProjDirPath}/src/01_hw_platform/CMSIS/cmsis_version.h"
"${ProjDirPath}/src/01_hw_platform/CMSIS/cmsis_gcc.h"
"${ProjDirPath}/src/01_hw_platform/CMSIS/cmsis_compiler.h"
"${ProjDirPath}/src/04_services/utilities/str/fsl_str.h"
"${ProjDirPath}/src/04_services/utilities/str/fsl_str.c"
"${ProjDirPath}/src/04_services/utilities/debug_console_lite/fsl_debug_console.h"
"${ProjDirPath}/src/04_services/utilities/debug_console_lite/fsl_debug_console.c"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/timers.c"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/tasks.c"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/stream_buffer.c"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/queue.c"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/list.c"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/event_groups.c"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/croutine.c"
"${ProjDirPath}/src/04_services/component/uart/fsl_adapter_uart.h"
"${ProjDirPath}/src/04_services/component/uart/fsl_adapter_lpuart.c"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_XTALOSC24M.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_XBARB.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_XBARA.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_WDOG.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_USDHC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_USB_ANALOG.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_USBPHY.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_USBNC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_USB.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_TSC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_TRNG.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_TMR.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_TEMPMON.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_SRC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_SPDIF.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_SNVS.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_SEMC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_RTWDOG.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_ROMC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_PXP.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_PWM.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_PMU.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_PIT.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_PGC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_OCOTP.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_LPUART.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_LPSPI.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_LPI2C.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_LCDIF.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_KPP.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_IOMUXC_SNVS_GPR.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_IOMUXC_SNVS.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_IOMUXC_GPR.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_IOMUXC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_I2S.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_GPT.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_GPIO.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_GPC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_FLEXSPI.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_FLEXRAM.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_FLEXIO.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_EWM.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_ENET.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_ENC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_DMAMUX.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_DMA.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_DCP.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_DCDC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_CSU.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_CSI.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_CMP.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_CM7_MCM.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_CCM_ANALOG.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_CCM.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_CAN.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_BEE.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_AOI.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_AIPSTZ.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_ADC_ETC.h"
"${ProjDirPath}/src/01_hw_platform/device/periph/PERI_ADC.h"
"${ProjDirPath}/src/01_hw_platform/CMSIS/m-profile/cmsis_gcc_m.h"
"${ProjDirPath}/src/01_hw_platform/CMSIS/m-profile/armv7m_mpu.h"
"${ProjDirPath}/src/01_hw_platform/CMSIS/m-profile/armv7m_cachel1.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/timers.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/task.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/stream_buffer.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/stack_macros.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/StackMacros.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/semphr.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/queue.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/projdefs.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/portable.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/mpu_wrappers.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/mpu_syscall_numbers.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/mpu_prototypes.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/message_buffer.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/list.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/freertos_tasks_c_additions.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/FreeRTOS.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/event_groups.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/deprecated_definitions.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/croutine.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/include/atomic.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/portable/GCC/ARM_CM4F/portmacro.h"
"${ProjDirPath}/src/03_os/freertos/freertos-kernel/portable/GCC/ARM_CM4F/port.c"
)




if(${LIBRARY_TYPE} STREQUAL "REDLIB")
    target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE $ENV{ARMGCC_DIR}/redlib/include)
endif()

set(asm_include_directories "")


target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:ASM>:${asm_include_directories}>)

set(c_include_directories "${ProjDirPath}/../iMXRT1060/src"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/CMSIS"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/CMSIS/m-profile"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/device"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/device/periph"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/startup"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/xip"
"${ProjDirPath}/../iMXRT1060/src/02_mcal"
"${ProjDirPath}/../iMXRT1060/src/02_mcal/board"
"${ProjDirPath}/../iMXRT1060/src/02_mcal/drivers"
"${ProjDirPath}/../iMXRT1060/src/03_os"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel/include"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel/portable"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel/portable/GCC"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel/portable/GCC/ARM_CM4F"
"${ProjDirPath}/../iMXRT1060/src/04_services"
"${ProjDirPath}/../iMXRT1060/src/04_services/component"
"${ProjDirPath}/../iMXRT1060/src/04_services/component/uart"
"${ProjDirPath}/../iMXRT1060/src/04_services/config"
"${ProjDirPath}/../iMXRT1060/src/04_services/diagnostics"
"${ProjDirPath}/../iMXRT1060/src/04_services/utilities"
"${ProjDirPath}/../iMXRT1060/src/04_services/utilities/debug_console_lite"
"${ProjDirPath}/../iMXRT1060/src/04_services/utilities/str"
"${ProjDirPath}/../iMXRT1060/src/05_app"
"${ProjDirPath}/../iMXRT1060/src/02_mcal/hal"
)


target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:C>:${c_include_directories}>)

set(cpp_include_directories "${ProjDirPath}/../iMXRT1060/src"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/CMSIS"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/CMSIS/m-profile"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/device"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/device/periph"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/startup"
"${ProjDirPath}/../iMXRT1060/src/01_hw_platform/xip"
"${ProjDirPath}/../iMXRT1060/src/02_mcal"
"${ProjDirPath}/../iMXRT1060/src/02_mcal/board"
"${ProjDirPath}/../iMXRT1060/src/02_mcal/drivers"
"${ProjDirPath}/../iMXRT1060/src/03_os"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel/include"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel/portable"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel/portable/GCC"
"${ProjDirPath}/../iMXRT1060/src/03_os/freertos/freertos-kernel/portable/GCC/ARM_CM4F"
"${ProjDirPath}/../iMXRT1060/src/04_services"
"${ProjDirPath}/../iMXRT1060/src/04_services/component"
"${ProjDirPath}/../iMXRT1060/src/04_services/component/uart"
"${ProjDirPath}/../iMXRT1060/src/04_services/config"
"${ProjDirPath}/../iMXRT1060/src/04_services/diagnostics"
"${ProjDirPath}/../iMXRT1060/src/04_services/utilities"
"${ProjDirPath}/../iMXRT1060/src/04_services/utilities/debug_console_lite"
"${ProjDirPath}/../iMXRT1060/src/04_services/utilities/str"
"${ProjDirPath}/../iMXRT1060/src/05_app"
"${ProjDirPath}/../iMXRT1060/src/02_mcal/hal"
)


target_include_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE $<$<COMPILE_LANGUAGE:CXX>:${cpp_include_directories}>)

set(include_files "")


target_precompile_headers(${MCUX_SDK_PROJECT_NAME} PRIVATE ${include_files})

set(link_directories "")


set(link_libraries "")


target_link_directories(${MCUX_SDK_PROJECT_NAME} PRIVATE ${link_directories})
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--start-group)
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE ${TARGET_LINK_SYSTEM_LIBRARIES} ${link_libraries})
target_link_libraries(${MCUX_SDK_PROJECT_NAME} PRIVATE -Wl,--end-group)





