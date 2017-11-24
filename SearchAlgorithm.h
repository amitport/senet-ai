/*
 * SearchAlgorithm.h
 *
 *  Created on: Dec 17, 2009
 *      Author: shaharch
 */

#ifndef SEARCHALGORITHM_H_
#define SEARCHALGORITHM_H_

#include "SenetMove.h"
#include "GameTimer.h"
#include "Definitions.h"
#include "Utility.h"
#include "BoardHelper.h"

namespace senet_ai {

	typedef struct
	{
		SenetMove move;
		double hval;
	} SearchResult;

	/**
	 * an interace for our searh algorithms (expectimax, star1, star2)
	 */
	class SearchAlgorithm {
	public:
		typedef enum {PROB,DETER} SearchState;
		SearchAlgorithm() {};
		virtual ~SearchAlgorithm() {};

		//initiate the search variable and the probability vector
		void initSearch(Utility *utility, Utility *m_childOrderUtil, Definitions::PlayerColor color, int num_sticks);
		virtual SearchResult search(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha=0, double beta=0) = 0;

		static const int SEARCH_TIMEOUT = -2;
	protected:
		Definitions::PlayerColor the_color;
		std::vector<double> probs;   //probability vector (calculated only once)
		Utility *m_utility;     //main utility used in the search
		Utility *m_lightUtil;   //utility used in successors sorting
		int m_num_sticks;	    // The number of sticks
	};
}
#endif /* SEARCHALGORITHM_H_ */
