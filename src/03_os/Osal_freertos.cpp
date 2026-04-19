/**
 * @file Osal_freertos.cpp
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


#include "Osal_rtos.hpp"

extern "C" {
	#include "FreeRTOS.h"
	#include "task.h"
	#include "semphr.h"
	#include "queue.h"
}

extern "C" {

    void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                       StackType_t **ppxIdleTaskStackBuffer,
                                       size_t *pulIdleTaskStackSize)
    {
    	static StaticTask_t xIdleTaskTCB;
		static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

        *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;
        *ppxIdleTaskStackBuffer = uxIdleTaskStack;
        *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
    }

    #if(configUSE_TIMERS == 1)
		static StaticTask_t xTimerTaskTCB;
		static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

		void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
											StackType_t **ppxTimerTaskStackBuffer,
											uint32_t *pulTimerTaskStackSize) {
			*ppxTimerTaskTCBBuffer = &xTimerTaskTCB;
			*ppxTimerTaskStackBuffer = uxTimerTaskStack;
			*pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
		}
    #endif
}

namespace Osal {

	namespace Kernel {
		void Start(void)
		{
			vTaskStartScheduler();
		}

		void Lock(void)
		{
			vTaskSuspendAll();
		}

		void Unlock(void)
		{
			xTaskResumeAll();
		}

		uint64_t GetUptimeMS(void)
		{
			return ((uint64_t)((xTaskGetTickCount() * 1000) / configTICK_RATE_HZ));
		}


	}

	//Mapping OSAL priorities to freeRTOS priorities
	static UBaseType_t MapPriority (Priority p)
	{
		UBaseType_t ret = tskIDLE_PRIORITY;
		switch(p)
		{
			case Priority::enumIdle:		ret = tskIDLE_PRIORITY; break;
			case Priority::enumLow:			ret = 1; break;
			case Priority::enumNormal:		ret = (configMAX_PRIORITIES/2); break;
			case Priority::enumHigh:		ret = (configMAX_PRIORITIES - 2); break;
			case Priority::enumRealTime:	ret = (configMAX_PRIORITIES - 1); break;
			default:						ret = (configMAX_PRIORITIES / 2); break;
		}

		return ret;
	}

	static TickType_t ToTicks(std::chrono::milliseconds ms)
	{
		if(ms == std::chrono::milliseconds::max()) return portMAX_DELAY;
		{
			return pdMS_TO_TICKS(ms.count());
		}
	}

	// Safety check: Ensure our abstract TCB is big enough for FreeRTOS
	static_assert(sizeof(ThreadControlBlock) >= sizeof(StaticTask_t),
	                  "ThreadControlBlock storage too small for FreeRTOS");

	Thread::Thread(taskFunction func, void* arg, Priority priority,
			uint32_t stackSize, void* stackBuffer, void* tcbBuffer,
			const std::string& name)
		:_func(func), _arg(arg), _prio(priority), _stackSz(stackSize),
		 _stackBuf(stackBuffer), _tcb(static_cast<ThreadControlBlock*>(tcbBuffer)), _name(name),
		 _handle(nullptr)
	{

	}

	Status Thread::Start()
	{
		Status ret = Status::enumError;
		_handle = xTaskCreateStatic(
				this->_func,
				this->_name.c_str(),
				(configSTACK_DEPTH_TYPE)(this->_stackSz),
				this->_arg,
				MapPriority(this->_prio),
				static_cast<StackType_t*>(this->_stackBuf),
				reinterpret_cast<StaticTask_t*>(this->_tcb)
			);
		if(_handle != nullptr)
		{
			ret = Status::enumOK;
		}

		return ret;
	}

	void Thread::Suspend(void)
	{
		if (this->_handle)
		{
			vTaskSuspend((TaskHandle_t)this->_handle);
		}
	}

	void Thread::Resume(void)
	{
		if (this->_handle)
		{
			vTaskResume((TaskHandle_t)this->_handle);
		}
	}

	void Thread::Sleep(std::chrono::milliseconds ms)
	{
		vTaskDelay(pdMS_TO_TICKS(ms.count()));
	}

	Thread::~Thread()
	{
		if (this->_handle != nullptr)
		{
			vTaskDelete((TaskHandle_t)this->_handle);
		}
	}

	//Mutex implementation
	Mutex::Mutex(SyncControlBlock& scb) : _handle(nullptr), _scb(scb)
	{
		_handle = xSemaphoreCreateMutexStatic(reinterpret_cast<StaticSemaphore_t*>(&_scb));

		if(_handle == nullptr)
		{
			//TODO: trigger a system halt or log error
		}
	}

	Mutex::~Mutex()
	{
		if(_handle != nullptr)
		{
			vSemaphoreDelete(static_cast<SemaphoreHandle_t> (_handle));
			_handle = nullptr;
		}

	}

	void Mutex::Lock(uint32_t timeout)
	{
		xSemaphoreTake(static_cast<SemaphoreHandle_t>(_handle), (TickType_t)(timeout));
	}

	void Mutex::Unlock()
	{
		xSemaphoreGive(static_cast<SemaphoreHandle_t>(_handle));
	}

	//Semaphore Implementation
	Semaphore::Semaphore(SyncControlBlock& scb, uint32_t initCount):_handle(nullptr), _scb(scb)
	{
		_handle = xSemaphoreCreateCountingStatic(100, initCount, reinterpret_cast<StaticSemaphore_t*>(&_scb));

		if(_handle == nullptr)
		{
			//TODO: Trigger a system fault or log error
		}
	}

	Semaphore::~Semaphore()
	{
		vSemaphoreDelete(static_cast<SemaphoreHandle_t>(_handle));
		_handle = nullptr;
	}

	void Semaphore::Post(void)
	{
		xSemaphoreGive(static_cast<SemaphoreHandle_t>(_handle));
	}

	Status Semaphore::wait(std::chrono::milliseconds timeout)
	{
		return(xSemaphoreTake(static_cast<SemaphoreHandle_t>(_handle), ToTicks(timeout)) == pdTRUE) ? Status::enumOK : Status::enumTimeout;
	}

	//Queue Implementation
	QueueBase::QueueBase(uint32_t length, uint32_t itemSize, void* storageBuff, QueueControlBlock& qcb)
	{
		//storageBuff, qcb
		_handle = xQueueCreateStatic(length, itemSize, static_cast<uint8_t*>(storageBuff), reinterpret_cast<StaticQueue_t*>(&qcb));

		if(_handle == nullptr)
		{
			//TODO: Trigger a system fault or log error
		}
	}

	QueueBase::~QueueBase()
	{
		vQueueDelete(static_cast<QueueHandle_t>(_handle));
		_handle = nullptr;
	}

	Status QueueBase::Push(const void* item, std::chrono::milliseconds timeout)
	{
		return (xQueueSend(static_cast<QueueHandle_t>(_handle), item, ToTicks(timeout)) == pdTRUE) ? Status::enumOK : Status::enumTimeout;
	}

	Status QueueBase::Pop(void* item, std::chrono::milliseconds timeout)
	{
		return (xQueueReceive(static_cast<QueueHandle_t>(_handle), item, ToTicks(timeout))) == pdTRUE ? Status::enumOK : Status::enumTimeout;
	}





} //namespace OSAL
