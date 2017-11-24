/*
 * star2.h
 *
 *  Created on: Dec 20, 2009
 *      Author: shaharch
 */

#ifndef STAR2_H_
#define STAR2_H_

#include "SearchAlgorithm.h"

class Star2: public SearchAlgorithm {
public:
	Star2() {srand ( time(NULL) );}
	virtual ~Star2() {}
	SearchResult search(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha=0, double beta=0);
	SearchResult probe_with_son(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha=0, double beta=0);
private:
	int getRandInt(int M) {return rand()%M;}
};

#endif /* STAR2_H_ */
