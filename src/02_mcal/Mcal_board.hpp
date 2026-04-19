/**
 * @file Mcal_board.hpp
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
#ifndef __MCAL_BOARD_HPP_
#define __MCAL_BOARD_HPP_



namespace Mcal{

	class Board{
	public:

		Board(); //constructor
		~Board(); //distructor

		static void InitHardware(void);
	};
}



#endif /* 02_MCAL_MCAL_BOARD_HPP_ */
