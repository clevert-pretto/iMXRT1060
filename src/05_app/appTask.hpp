/**
 * @file appTask.hpp
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
#ifndef __APPTASK_HPP_
#define __APPTASK_HPP_

#include "Osal_rtos.hpp"

namespace AppTask {
    // Share the pointer globally
    extern Osal::StaticThread<2048>* helloTaskPtr;
}



#endif /* __APPTASK_HPP_ */
