/*
 * Expectimax.cpp
 *
 *  Created on: Dec 17, 2009
 *      Author: shaharch
 */

#include "Expectimax.h"
#include "SenetSmart1Player.h"
#include "SenetBoard.h"
#include "SenetMove.h"
#include "BoardHelper.h"

SearchResult Expectimax::search(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha, double beta)
{
	SearchResult res, son_res;

	if (timer.isMoveTimePassed(GameTimer::NO_OVERHEAD)){
		res.move.m_square = SEARCH_TIMEOUT;
		return res;
	}
	// End of recursion - depth limit, final state, or time limit
	if ((curNode.m_board->getBoardResult() != SenetBoard::NOT_FINAL) || currentDepth==maxDepth) {
		//we wont really reach here while we are using the same util
		res.hval = curNode.m_hval;
		//res.hval = m_utility->calculate(*(curNode.m_board),the_color);
		return res;
	}

	// Probabilistic state
	if (state == SearchAlgorithm::PROB)
	{
		vector<BoardNode*>* nextBoards = BoardHelper::makeMovesAndSortProb(&probs,curNode);
		res.hval = 0;
		for (vector<BoardNode*>::iterator iter = nextBoards->begin();
				iter != nextBoards->end();
				iter++)
		{
			son_res = search(*(*iter),currentDepth,maxDepth,SearchAlgorithm::DETER,timer);
			if (son_res.move.m_square == SEARCH_TIMEOUT){
				return son_res;
			}
			res.hval += (*iter)->m_hval*son_res.hval; //the hval in the current element is the probability !
		}
		return res;
	}

	currentDepth++;
	// Deterministic state
	vector<BoardNode*>* nextBoards = BoardHelper::makeMovesAndSort(m_utility, curNode, the_color);
	vector<SenetMove> best_succ;
	double best_val;

	// Get max sons
	if (curNode.m_board->getCurPlayerColor() == the_color)
	{
		if (currentDepth == maxDepth){
			//while we are using the same util no need to run it twice
			best_succ.push_back((*nextBoards)[0]->m_move);
			best_val = (*nextBoards)[0]->m_hval;
		} else {
			best_val = -std::numeric_limits<double>::infinity();
			for (vector<BoardNode*>::iterator iter = nextBoards->begin();
					iter != nextBoards->end();
					iter++)
			{
				son_res = search(*(*iter),currentDepth,maxDepth,SearchAlgorithm::PROB,timer);
				if (son_res.move.m_square == SEARCH_TIMEOUT){
					return son_res;
				}

				if (son_res.hval > best_val)
				{
					best_val = son_res.hval;
					best_succ.clear();
					best_succ.push_back((*iter)->m_move);
				}
				else if (son_res.hval == best_val)
				{
					best_succ.push_back((*iter)->m_move);
				}
			}
		}
	}
	else // curBoard.getCurPlayerColor() != the_color
	{
		if (currentDepth == maxDepth){
			//while we are using the same util no need to run it twice
			best_succ.push_back((*nextBoards)[(nextBoards->size()-1)]->m_move);
			best_val = (*nextBoards)[(nextBoards->size()-1)]->m_hval;
		} else {
			best_val = std::numeric_limits<double>::infinity();
			//iterate from the end ! get the lower values first
			for (vector<BoardNode*>::reverse_iterator iter = nextBoards->rbegin();
					iter != nextBoards->rend();
					iter++)
			{
				son_res = search(*(*iter),currentDepth,maxDepth,SearchAlgorithm::PROB,timer);
				if (son_res.move.m_square == SEARCH_TIMEOUT){
					return son_res;
				}

				if (son_res.hval < best_val)
				{
					best_val = son_res.hval;
					best_succ.clear();
					best_succ.push_back((*iter)->m_move);
				}
				else if (son_res.hval == best_val)
				{
					best_succ.push_back((*iter)->m_move);
				}
			}
		}
	}

	// Return best sons
	res.hval = best_val;
	res.move = best_succ[0]; // Maybe should change to random son
	return res;
}
