/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

//#include "Mcal_post.hpp"
#include "Mcal_board.hpp"
#include "Osal_rtos.hpp"
#include "appTask.hpp"
#include "svl_postManager.hpp"
#include "Svl_logger.hpp"
/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
extern "C" void __libc_init_array(void); // prototype for standard init function
static void hello_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */
extern "C" {
	#include "fsl_device_registers.h"
	#include "fsl_common.h"
}
int main(void)
{
	/* Force Enable FPU (Coprocessor 10 and 11) */
	SCB->CPACR |= ((3UL << 10*2) | (3UL << 11*2));
	__DSB();
	__ISB();
    /* Init board hardware. */
    Mcal::Board::InitHardware();

    SDK_DelayAtLeastUs(1000, SDK_DEVICE_MAXIMUM_CPU_CLOCK_FREQUENCY);
    //Check reset cause
    Mcal::ResetCause resetCause = Service::PostManager::RunPhase1ResetCauseTest();

    //Run phase 1 critical CPU core test
    uint32_t ph1Results = Service::PostManager::RunPhase1CriticalCoreTest();
    if(Mcal::PostStatus::enumPASS != static_cast<Mcal::PostStatus>(ph1Results))
    {
    	//TODO: Handle FATAL ERROR, Blink LED pattern ?
    	while(1);
    }

    __libc_init_array();

    Service::Logger::Init();

    Service::Logger::Log(Service::LogLevel::enumInfo, "Reset cause = %d\r\n", resetCause);
    Service::Logger::Log(Service::LogLevel::enumInfo, "Phase 1: Core CPU Test PASSED\r\n", resetCause);

    Osal::StaticThread<2048> helloTask(hello_task,
    								   nullptr,
    								   Osal::Priority::enumNormal,
    								   "Hello_task");

    AppTask::helloTaskPtr = &helloTask;

    if(AppTask::helloTaskPtr->Start() == Osal::Status::enumError)
    {
        //PRINTF("App Task creation failed!.\r\n");

        Service::Logger::Log(Service::LogLevel::enumInfo, "App Task creation failed!.\r\n");
        while (1)
            ;
    }
    Osal::Kernel::Start();
    for (;;)
        ;
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
void hello_task(void *pvParameters)
{
    uint32_t counter = 0;

    // Use the single-entry Log method with printf-style arguments
    Service::Logger::Log(Service::LogLevel::enumInfo, "Task started. Pointer address: %p", pvParameters);

    for (;;)
    {
        Service::Logger::Log(Service::LogLevel::enumInfo, "System Heartbeat - Count: %d", counter++);

        /* Example of using different log levels */
        if (counter % 10 == 0)
        {
            Service::Logger::Log(Service::LogLevel::enumWarning, "Counter reached a multiple of 10.");

        }
        Osal::Thread::Sleep(std::chrono::milliseconds(1000));

        if (counter > 10)
		{
			/* Use OSAL for delay to keep logic agnostic */
        	AppTask::helloTaskPtr->Suspend();
		}
    }
}
