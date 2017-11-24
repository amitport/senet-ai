/*
 * SmartUtility.h
 *
 *  Created on: Dec 24, 2009
 *      Author: shaharch
 */

#ifndef SmartUtility_H_
#define SmartUtility_H_

#include "Utility.h"

namespace senet_ai{
	/**
	 * our complex utility (see "calculate"'s implementation)
	 */
	class SmartUtility: public Utility {
	public:
		SmartUtility();
		// Does nothing
		virtual ~SmartUtility() {};
		// Calculate utility function for suggested board
		double calculate(const SenetBoard& curBoard, Definitions::PlayerColor playerColor);
		void initialize(int size_squares, int num_pieces, int num_sticks);
	private:
		std::vector<double> probs;
	};
}

#endif /* SmartUtility_H_ */
