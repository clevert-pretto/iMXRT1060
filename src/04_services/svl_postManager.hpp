/**
 * @file svl_postManager.hpp
 * @author ravic
 * @brief TODO
 * @date 22-Apr-2026
 * * @copyright Copyright (c) 2026 ravic. All rights reserved.
 * * @attention
 * This software is the confidential and proprietary information of ravic.
 * Unauthorized copying of this file, via any medium, is strictly prohibited.
 * Use, distribution, or modification of this code is permitted only under 
 * the terms of the formal license agreement provided by the copyright holder.
 */
#ifndef __SVL_POSTMANAGER_HPP_
#define __SVL_POSTMANAGER_HPP_

#include "Mcal_post.hpp"

namespace Service {

	class PostManager {
	public:
		static Mcal::ResetCause RunPhase1ResetCauseTest (void);
		static uint32_t RunPhase1CriticalCoreTest (void);
		static bool RunPhase2InternalPeriBusTest (void);
		static bool RunPhase3InternalSysAcceltest (void);
		static bool RunPhase4ExternalFunctionalTest (void);

	};
}







#endif /* __SVL_POSTMANAGER_HPP_ */
