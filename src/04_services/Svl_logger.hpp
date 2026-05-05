/**
 * @file Svl_logger.hpp
 * @author ravic
 * @brief Asynchronous Logger Service for MIMXRT1060
 * @date 20-Apr-2026
 * * @copyright Copyright (c) 2026 ravic. All rights reserved.
 * * @attention
 * This software is the confidential and proprietary information of ravic.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Use, distribution, or modification of this code is permitted only under 
 * the terms of the formal license agreement provided by the copyright holder.
 */
#ifndef __SVL_LOGGER_HPP_
#define __SVL_LOGGER_HPP_

#include "Osal_rtos.hpp"
#include <cstdarg>

#define SERVICE_LOGGER_BUFFER_LEN		128U	// Bytes
#define	SERVICE_LOGGER_QUEUE_LEN		16U		// numbers
#define SERVICE_LOGGER_TASK_STACK_SIZE	4096U 	// Bytes
#define SERVICE_LOGGER_LOCK_TIMEOUT		10U 	// Milliseconds
#define SERVICE_LOGGER_BLOCK_TIME		2000U 	// Milliseconds

namespace Service {

	/**
     * @brief Log levels to categorize system events.
     */
    enum class LogLevel {
        enumInfo,
        enumWarning,
        enumError,
        enumMemoryOnly  // For high-speed trace captured in RAM only
    };

    /**
	 * @brief Internal structure for log items passed through the queue.
	 * Fits comfortably on the caller's stack (approx 132 bytes).
	 */
	struct LogItem {
		LogLevel level;
		char buffer[SERVICE_LOGGER_BUFFER_LEN];
	};

	class Logger {
	public:
		/**
		 * @brief Initializes the logging queue and consumer thread
		 * must be called before starting the OS kernel
		 */
		static void Init();

		/**
		 * @brief Primary entry point for logging. Handles printf-style arguments.
		 * @param level The severity level of the log.
		 * @param format The format string (e.g., "Value: %d").
		 * @param ... Variadic arguments for the format string.
		 */
		static void Log(LogLevel level, const char* format, ...);

	private:

		/**
		 * @brief Background task that pulls items from the queue and prints them.
		 */
		static void LogConsumerTask(void* params);

		//OSAL static objects ensures no heap allocation is used
		static Osal::StaticQueue<LogItem, SERVICE_LOGGER_QUEUE_LEN> _logQueue;
		static Osal::StaticThread<SERVICE_LOGGER_TASK_STACK_SIZE> _logThread;
		static Osal::StaticSemaphore _logLock;
	};
}



#endif /* __SVL_LOGGER_HPP_ */
