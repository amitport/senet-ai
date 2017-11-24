/*
 * Star1.h
 *
 *  Created on: Dec 19, 2009
 *      Author: shaharch
 */

#ifndef STAR1_H_
#define STAR1_H_

#include "SearchAlgorithm.h"

namespace senet_ai{
	/**
	 * our Star1 implementation
	 */
	class Star1: public SearchAlgorithm {
	public:
		Star1() {}
		virtual ~Star1() {}
		SearchResult search(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha, double beta);
	};
}

#endif /* STAR1_H_ */
