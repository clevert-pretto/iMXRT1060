/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2017 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

extern "C"{
/* Freescale includes. */
#include "fsl_device_registers.h"
#include "fsl_debug_console.h"

}

#include "Mcal_board.hpp"
#include "Osal_rtos.hpp"
#include "appTask.hpp"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
/* Task priorities. */

#define hello_task_PRIORITY   4//(configMAX_PRIORITIES - 1)
/*******************************************************************************
 * Prototypes
 ******************************************************************************/
static void hello_task(void *pvParameters);

/*******************************************************************************
 * Code
 ******************************************************************************/
/*!
 * @brief Application entry point.
 */




int main(void)
{
    /* Init board hardware. */
    Mcal::Board::InitHardware();

    Osal::StaticThread<2048> helloTask(hello_task,
    								   nullptr,
    								   Osal::Priority::enumNormal,
    								   "Hello_task");

    AppTask::helloTaskPtr = &helloTask;

    if(AppTask::helloTaskPtr->Start() == Osal::Status::enumError)
//    if (xTaskCreate(hello_task, "Hello_task", configMINIMAL_STACK_SIZE + 100, NULL, hello_task_PRIORITY, NULL) !=
//        pdPASS)
    {
        PRINTF("App Task creation failed!.\r\n");
        while (1)
            ;
    }
    Osal::Kernel::Start();
    //vTaskStartScheduler();
    for (;;)
        ;
}

/*!
 * @brief Task responsible for printing of "Hello world." message.
 */
void hello_task(void *pvParameters)
{
    for (;;)
    {
        PRINTF("Hello world.\r\n");
//        Osal::Thread::Sleep(std::chrono::milliseconds(1000));
        AppTask::helloTaskPtr->Suspend();
        //helloTask.Suspend();
        //vTaskSuspend(NULL);
    }
}
