/*
 * Expectimax.h
 *
 *  Created on: Dec 17, 2009
 *      Author: shaharch
 */

#ifndef EXPECTIMAX_H_
#define EXPECTIMAX_H_

#include "SearchAlgorithm.h"

class Expectimax: public SearchAlgorithm {
public:
	Expectimax() {};
	virtual ~Expectimax() {};
	SearchResult search(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha=0, double beta=0);
};

#endif /* EXPECTIMAX_H_ */
