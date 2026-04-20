/**
 * @file Svl_logger.cpp
 * @author ravic
 * @brief TODO
 * @date 20-Apr-2026
 * * @copyright Copyright (c) 2026 ravic. All rights reserved.
 * * @attention
 * This software is the confidential and proprietary information of ravic.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Use, distribution, or modification of this code is permitted only under 
 * the terms of the formal license agreement provided by the copyright holder.
 */

#include "Svl_logger.hpp"
#include <cstdio>

extern "C" {
	#include "fsl_debug_console.h"
}

namespace Service {

	//Define static members using OSAL control blocks
	Osal::StaticQueue<LogItem, SERVICE_LOGGER_QUEUE_LEN> Logger::_logQueue;
	Osal::StaticSemaphore Logger::_logLock(1);
	Osal::StaticThread<SERVICE_LOGGER_TASK_STACK_SIZE> Logger::_logThread(Logger::LogConsumerTask, nullptr, Osal::Priority::enumLow, "LogTask");

	void Logger::Init()
	{
		//Start the background consumer thread
		_logThread.Start();
	}


	void Logger::Log(LogLevel level, const char* format, ...)
	{
		LogItem item;
		item.level = level;

		//Use mutex only during formatting to prevent garbled strings if multiple tasks log simultaneously
		if(_logLock.wait(std::chrono::milliseconds(SERVICE_LOGGER_LOCK_TIMEOUT)) == Osal::Status::enumOK) {
			va_list args;
			va_start (args, format);

			// vsnprintf captures the format sepcifier (%d, %f)
			vsnprintf(item.buffer, sizeof(item.buffer), format, args);

			va_end(args);
			_logLock.Post(); // Release lock for other producers
		}
		else
		{
			//Optional TODO: Handle case where logger is too busy to format
		}

		// Send to queue. Non-blocking (0 timeout)
		_logQueue.Send(item, std::chrono::milliseconds(0));
	}


	void Logger::LogConsumerTask(void* params)
	{
		LogItem item;

		for (;;)
		{
			// Block untill a message is available in the queue

			if(_logQueue.Receive(item, std::chrono::milliseconds(SERVICE_LOGGER_BLOCK_TIME)) == Osal::Status::enumOK)
			{
				switch(item.level)
				{
					case LogLevel::enumInfo:
						DbgConsole_Printf("[INFO] %s\r\n", item.buffer);
					break;
					case LogLevel::enumWarning:
						DbgConsole_Printf("[WARN] %s\r\n", item.buffer);
					break;
					case LogLevel::enumError:
						DbgConsole_Printf("[ERROR] %s\r\n", item.buffer);
					break;
					case LogLevel::enumMemoryOnly:
						// TODO : For RT1060, write to a specific SRAM section
					break;

				}
			}
		}
	}

}


