/**
 * @file Osal_rtos.hpp
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
#ifndef __OSAL_RTOS_HPP_
#define __OSAL_RTOS_HPP_

#include <cstdint>
#include <chrono>
#include <string>

namespace Osal {

	namespace Kernel {
		//Start the underlying RTOS Scheduler
		void Start(void);

		//Global lock/unlock (Disable/Enable interrupts or scheduler)
		void Lock(void);
		void Unlock(void);

		//Get system uptime
		uint64_t GetUptimeMS(void);

	}

	// Standardized priorities: We will wrap these to RTOS-specific values in .cpp
	enum class Priority {
		enumIdle = 0,
		enumLow,
		enumBelowNormal,
		enumNormal,
		enumAboveNormal,
		enumHigh,
		enumRealTime
	};

	enum class Status{
		enumOK = 0,
		enumTimeout,
		enumError,
		enumFull,
		enumEmpty
	};

	// Opaque Storage for RTOS Control Blocks
	// We reserve 128 bytes to be safe for both FreeRTOS (StaticTask_t)
	// and Eclipse ThreadX (TX_THREAD).
	struct ThreadControlBlock {
		uint64_t storage[16];
	};

	// Opaque storage for Mutex and Semaphore (FreeRTOS StaticSemaphore_t)
	struct SyncControlBlock {
		uint64_t storage[10];
	};

	// Opaque storage for Queue (FreeRTOS StaticQueue_t)
	struct QueueControlBlock {
		uint64_t storage[12];
	};

	//Thread Interface
	class Thread {
		public:

			using taskFunction = void(*) (void*);

			Thread (taskFunction func,
					void* arg,
					Priority priority,
					uint32_t stackSize,
					void* stackBuffer,  //pointer to pre-allocated stack buffer
					void* tcbBuffer,  //pointer to pre-allocated TCB buffer
					const std::string& name = "Task");

			virtual ~Thread();

			Status Start();
			void Suspend(void);
			void Resume(void);

			static void Sleep(std::chrono::milliseconds ms);
			Status Join(std::chrono::milliseconds timeout); //Wait for thread to finish

		protected:
			taskFunction _func;
			void* _arg;
			Priority _prio;
			uint32_t _stackSz;
			void* _stackBuf;
			ThreadControlBlock* _tcb;
			std::string _name;
			void* _handle;
	};

	//Mutex Interface for RAII
	class Mutex {
		public:
			Mutex(SyncControlBlock& scb);
			~Mutex();

			void Lock(uint32_t timeout = 0xFFFFFFFF);
			bool TryLock(std::chrono::milliseconds timeout);
			void Unlock();
		private:
			void* _handle;
			SyncControlBlock& _scb;
	};

	class StaticMutex : public Mutex {
		public:
			StaticMutex() : Mutex(scbMem){}
		private:
			SyncControlBlock scbMem;
	};

	class Semaphore{
		public:
			Semaphore(SyncControlBlock& scb, uint32_t initCount = 0);
			~Semaphore();
			void Post(void); //Signal
			Status wait(std::chrono::milliseconds timeout);
		private:
			void* _handle;
			SyncControlBlock& _scb;
	};

	class StaticSemaphore : public Semaphore {
		public:
			StaticSemaphore(uint32_t initCount = 0) : Semaphore(_scbMem, initCount){}

		private:
			SyncControlBlock _scbMem;
	};

	// Queue type-safe wrapper
	class QueueBase {
		protected:
			QueueBase(uint32_t length, uint32_t itemSize, void* storageBuff, QueueControlBlock& qcb);
			~QueueBase();

			Status Push(const void* item, std::chrono::milliseconds timeout);
			Status Pop(void* item, std::chrono::milliseconds timeout);

			void* _handle;
	};

	template <typename T, uint32_t Length>
	class StaticQueue : private QueueBase {
		public:
			StaticQueue():QueueBase(Length, sizeof(T), dataStorage, qcbMem){}
			Status Send(const T& item, std::chrono::milliseconds timeout = std::chrono::milliseconds(0))
			{
				return Push(&item, timeout);
			}
			Status Receive(T& item, std::chrono::milliseconds timeout = std::chrono::milliseconds(0))
			{
				return Pop(&item, timeout);
			}
		private:
			uint8_t dataStorage[Length * sizeof(T)];
			QueueControlBlock qcbMem;
	};

	template <uint32_t StaticStackSize>
	class StaticThread : public Thread {
		public:
			StaticThread(taskFunction func, void* arg, Priority priority,
							const std::string& name)
			    : Thread(func, arg, priority, (StaticStackSize / sizeof(uint32_t)), StackMem, &tcbMem, name){}

		private:
			//alignas(8) ensured the stack meets ARM architecture requirement
			alignas(8) uint32_t StackMem[(StaticStackSize / sizeof(uint32_t))];
			ThreadControlBlock tcbMem;
	};
}

#endif /* __OSAL_RTOS_HPP_ */
