/*
 * star2.cpp
 *
 *  Created on: Dec 20, 2009
 *      Author: shaharch
 */

#include "Star2.h"

SearchResult Star2::search(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha, double beta)
{
	SearchResult res, son_res;
	if (timer.isMoveTimePassed(GameTimer::NO_OVERHEAD)){
		res.move.m_square = SEARCH_TIMEOUT;
		return res;
	}

	// End of recursion - depth limit, final state, or time limit
	if ((curNode.m_board->getBoardResult() != SenetBoard::NOT_FINAL) || currentDepth==maxDepth)
	{
		res.hval = m_utility->calculate(*curNode.m_board,the_color);
		return res;
	}

	/////////////////////////
	// Probabilistic state //
	/////////////////////////
	if (state == SearchAlgorithm::PROB)
	{
		double L = m_utility->getLowerBound(); // make static in init
		double U = m_utility->getUpperBound();
		// udatedL is a lower bound for the procedure.
		// It is initialized with L - the absolute lower bound
		// In case of Max search it is updated with representative value of each son
		// Aftewards it is updated with real value of each son
		double updatedU = U, updatedL = L; // Updated bounds of the search
		if (curNode.m_board->getCurPlayerColor() == the_color)
		{
			// Generate "better_bounds" containing value of one of the sons for each prob event
			// updatedL contains lower bound of expectancy
			// In case of Max search this will be the new lower bound of each event (because we know we would like to maximize the event)
			vector <double> better_bounds(m_num_sticks+1);

			vector<BoardNode*>* nextBoards = BoardHelper::makeMovesAndSortProb(&probs,curNode);
			//chance state sorted from higher to lower probability
			for (vector<BoardNode*>::iterator iter = nextBoards->begin();
					iter != nextBoards->end();
					iter++)
			{
				//((*iter)->m_hval) is the probability of this chance state !
				double prob = ((*iter)->m_hval);
				int ON_sticks = (*iter)->m_board->getSticksState();

				double cur_beta = (beta - updatedL + prob*L)/prob;
				double bx = std::min(cur_beta,U);

				// get representative score from one of the predecessors
				son_res = probe_with_son(*(*iter), currentDepth,maxDepth, SearchAlgorithm::DETER, timer, L, bx);
				if (son_res.move.m_square == SEARCH_TIMEOUT){
					return son_res;
				}

				better_bounds[ON_sticks] = son_res.hval;
				updatedL += (son_res.hval - L)*prob;
				if (son_res.hval >= cur_beta)
				{
					res.hval = std::numeric_limits<double>::infinity();
					return res;
				}
			}

			//chance state sorted from higher to lower probability
			for (vector<BoardNode*>::iterator iter = nextBoards->begin();
					iter != nextBoards->end();
					iter++)
			{
				//((*iter)->m_hval) is the probability of this chance state !
				double prob = ((*iter)->m_hval);
				int ON_sticks = (*iter)->m_board->getSticksState();

				double cur_alpha = (alpha - updatedU + prob*U)/prob;
				double cur_beta = (beta - updatedL + prob*better_bounds[ON_sticks])/prob;
				double ax = std::max(L,cur_alpha);
				double bx = std::min(U,cur_beta);

				// Run search on each son with updated bounds
				son_res = search(*(*iter),currentDepth,maxDepth, SearchAlgorithm::DETER, timer, ax, bx);
				if (son_res.move.m_square == SEARCH_TIMEOUT){
					return son_res;
				}

				updatedL += (son_res.hval - better_bounds[ON_sticks])*prob;
				updatedU += (son_res.hval - U)*prob;

				if (son_res.hval >= cur_beta)
				{
					res.hval = std::numeric_limits<double>::infinity();//
					return res;
				}

				if (son_res.hval <= cur_alpha)
				{
					res.hval = -std::numeric_limits<double>::infinity();//
					return res;
				}
			}
		}

		else /* (curBoard.getCurPlayerColor() != the_color) */
		{
			// Generate "better_bounds" containing value of one of the sons for each prob event
			// updatedL contains upper bound of expectancy
			// In case of Min search this will be the new upper bound of each event (because we know we would like to minimize the event)
			vector <double> better_bounds(m_num_sticks+1);

			vector<BoardNode*>* nextBoards = BoardHelper::makeMovesAndSortProb(&probs,curNode);
			//chance state sorted from higher to lower probability
			for (vector<BoardNode*>::iterator iter = nextBoards->begin();
					iter != nextBoards->end();
					iter++)
			{
				//((*iter)->m_hval) is the probability of this chance state !
				double prob = ((*iter)->m_hval);
				int ON_sticks = (*iter)->m_board->getSticksState();

				double cur_alpha = (alpha - updatedU + prob*U)/prob;
				double ax = std::max(cur_alpha,L);

				// get representative score from one of the predecessors
				son_res = probe_with_son(*(*iter), currentDepth, maxDepth, SearchAlgorithm::DETER, timer, ax, U);
				if (son_res.move.m_square == SEARCH_TIMEOUT){
					return son_res;
				}

				better_bounds[ON_sticks] = son_res.hval;
				updatedU += (son_res.hval - U)*prob;
				if (son_res.hval <= cur_alpha)
				{
					res.hval = updatedU;
					return res;
				}
			}

			//chance state sorted from higher to lower probability
			for (vector<BoardNode*>::iterator iter = nextBoards->begin();
					iter != nextBoards->end();
					iter++)
			{
				//((*iter)->m_hval) is the probability of this chance state !
				double prob = ((*iter)->m_hval);
				int ON_sticks = (*iter)->m_board->getSticksState();

				double cur_alpha = (alpha - updatedU + prob*better_bounds[ON_sticks])/prob;
				double cur_beta = (beta - updatedL + prob*L)/prob;
				double ax = std::max(L,cur_alpha);
				double bx = std::min(U,cur_beta);
				// Run search on each son with updated bounds
				son_res = search(*(*iter),currentDepth,maxDepth, SearchAlgorithm::DETER, timer, ax, bx);
				if (son_res.move.m_square == SEARCH_TIMEOUT){
					return son_res;
				}

				updatedL += (son_res.hval - L)*prob;
				updatedU += (son_res.hval - better_bounds[ON_sticks])*prob;

				if (son_res.hval >= cur_beta)
				{
					res.hval = std::numeric_limits<double>::infinity();//
					return res;
				}

				if (son_res.hval <= cur_alpha)
				{
					res.hval = -std::numeric_limits<double>::infinity();//
					return res;
				}
			}
		}

		assert(updatedU == updatedL);
		res.hval = updatedL;
		return res;


	}

	/////////////////////////
	// Deterministic state //
	/////////////////////////
	currentDepth++;
	// Deterministic state
	vector<BoardNode*>* nextBoards = BoardHelper::makeMovesAndSort(m_utility, curNode, the_color);

	// Find Max under alpha-beta bounds
	if (curNode.m_board->getCurPlayerColor() == the_color)
	{
		res.hval = -std::numeric_limits<double>::infinity();
		for (unsigned int ind = 0; ind<(*nextBoards).size(); ind++)
		{
			son_res = search(*((*nextBoards)[ind]),currentDepth,maxDepth,SearchAlgorithm::PROB,timer,alpha,beta);
			if (son_res.move.m_square == SEARCH_TIMEOUT){
				return son_res;
			}

			if (son_res.hval > alpha)
			{
				if (son_res.hval >= beta)	// Maximum beyond bound, stop
				{
					res.hval = std::numeric_limits<double>::infinity();
					return res;
				}
				alpha = son_res.hval;
				res.hval = alpha;
				res.move = (*nextBoards)[ind]->m_move; // Found better Maximum bound
			}
		}
	}

	// Find Min under alpha-beta bounds
	else // curBoard.getCurPlayerColor() != the_color
	{
		res.hval = std::numeric_limits<double>::infinity();
		for (unsigned int ind = 0; ind<(*nextBoards).size(); ind++)
		{
			son_res = search(*((*nextBoards)[ind]),currentDepth,maxDepth,SearchAlgorithm::PROB,timer,alpha,beta);
			if (son_res.move.m_square == SEARCH_TIMEOUT){
				return son_res;
			}

			if (son_res.hval < beta)
			{
				if (son_res.hval <= alpha)	// Minimum beyond bound, stop
				{
					res.hval = -std::numeric_limits<double>::infinity();
					return res;
				}
				beta = son_res.hval;
				res.hval = beta;
				res.move = (*nextBoards)[ind]->m_move; // Found better Minimum bound
			}
		}
	}

	return res;
}

/*
 * Pick one of the sons and calculate its score. that's a limitation on the board's score
 * Think of a way to maximize the son... (son ordering and store results)
 */
SearchResult Star2::probe_with_son(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha, double beta)
{
	SearchResult res, son_res;
	if (timer.isMoveTimePassed(GameTimer::NO_OVERHEAD)){
		res.move.m_square = SEARCH_TIMEOUT;
		return res;
	}

	// End of recursion - depth limit or final state
	if ((curNode.m_board->getBoardResult() != SenetBoard::NOT_FINAL) || currentDepth==maxDepth)
	{
		res.hval = m_utility->calculate(*curNode.m_board,the_color);
		return res;
	}

	currentDepth++;
	// Deterministic state
	vector<BoardNode*>* nextBoards = BoardHelper::makeMovesAndSort(m_utility, curNode, the_color);

	int ind = getRandInt((*nextBoards).size()); // change to best son

	son_res = search(*((*nextBoards)[ind]),currentDepth,maxDepth,SearchAlgorithm::PROB,timer,-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
	if (son_res.move.m_square == SEARCH_TIMEOUT){
		return son_res;
	}

	res.move = (*nextBoards)[ind]->m_move;
	res.hval = son_res.hval;
	return res;
}
