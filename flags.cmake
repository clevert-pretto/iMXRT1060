if(NOT DEFINED FPU)
	set(FPU "-mfpu=fpv5-d16 -mfloat-abi=hard")
endif()

if(${LIBRARY_TYPE} STREQUAL "REDLIB")
	set(SPECS "-specs=redlib.specs")
elseif(${LIBRARY_TYPE} STREQUAL "NEWLIB_NANO")
	set(SPECS "--specs=nano.specs")
endif()

if(NOT DEFINED DEBUG_CONSOLE_CONFIG)
	set(DEBUG_CONSOLE_CONFIG "-DSDK_DEBUGCONSOLE=1")
endif()

set(CMAKE_ASM_FLAGS_DEBUG " \
    ${CMAKE_ASM_FLAGS_DEBUG} \
    ${FPU} \
    -mcpu=cortex-m7 \
    -mthumb \
")

set(CMAKE_C_FLAGS_DEBUG " \
    ${CMAKE_C_FLAGS_DEBUG} \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -std=c17 \
    -DXIP_BOOT_HEADER_DCD_ENABLE=1 \
    -DCPU_MIMXRT1062DVL6B \
    -DCPU_MIMXRT1062DVL6B_cm7 \
    -DMCUXPRESSO_SDK \
    -DXIP_BOOT_HEADER_ENABLE=1 \
    -DXIP_EXTERNAL_FLASH=1 \
    -DMCUX_META_BUILD \
    -DMIMXRT1062_SERIES \
    -DSDK_OS_FREE_RTOS \
    -D__USE_CMSIS \
    -DCR_INTEGER_PRINTF \
    -DPRINTF_FLOAT_ENABLE=0 \
    -D__MCUXPRESSO \
    -DDEBUG \
    -O0 \
    -fno-common \
    -fmerge-constants \
    -g3 \
     -ffunction-sections -fdata-sections -fno-builtin \
    -fstack-usage \
    -mcpu=cortex-m7 \
    -mthumb \
")

set(CMAKE_CXX_FLAGS_DEBUG " \
    ${CMAKE_CXX_FLAGS_DEBUG} \
    ${FPU} \
    ${DEBUG_CONSOLE_CONFIG} \
    -std=gnu++17 \
    -DCPU_MIMXRT1062DVL6B \
    -DXIP_BOOT_HEADER_DCD_ENABLE=1 \
    -DCPU_MIMXRT1062DVL6B_cm7 \
    -DMCUXPRESSO_SDK \
    -DXIP_BOOT_HEADER_ENABLE=1 \
    -DXIP_EXTERNAL_FLASH=1 \
    -DMCUX_META_BUILD \
    -DMIMXRT1062_SERIES \
    -DSDK_OS_FREE_RTOS \
    -DCR_INTEGER_PRINTF \
    -DPRINTF_FLOAT_ENABLE=0 \
    -D__MCUXPRESSO \
    -D__USE_CMSIS \
    -DDEBUG \
    -O0 \
    -fno-common \
    -fmerge-constants \
    -g3 \
    -Wall \
     -ffunction-sections -fdata-sections -fno-builtin -fno-rtti -fno-exceptions \
    -fstack-usage \
    -mcpu=cortex-m7 \
    -mthumb \
")

set(CMAKE_EXE_LINKER_FLAGS_DEBUG " \
    ${CMAKE_EXE_LINKER_FLAGS_DEBUG} \
    ${FPU} \
    ${SPECS} \
    -nostdlib \
    -Xlinker \
    -Map=output.map \
    -Xlinker \
    --gc-sections \
    -Xlinker \
    -print-memory-usage \
    -Xlinker \
    --sort-section=alignment \
    -Xlinker \
    --cref \
    -mcpu=cortex-m7 \
    -mthumb \
    -T\"${ProjDirPath}/iMXRT1060_Debug.ld\" \
")
