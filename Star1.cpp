/*
 * Star1.cpp
 *
 *  Created on: Dec 19, 2009
 *      Author: shaharch
 */

#include "Star1.h"

using namespace senet_ai;

SearchResult Star1::search(BoardNode &curNode, int currentDepth, int maxDepth, SearchState state, GameTimer &timer, double alpha, double beta)
{
	SearchResult res, son_res;

	if (timer.isMoveTimePassed(GameTimer::NO_OVERHEAD))
	{
		res.move.m_square = SearchAlgorithm::SEARCH_TIMEOUT;
		return res;
	}

	// End of recursion - depth limit, final state
	if ((curNode.boardResult != SenetBoard::NOT_FINAL) || currentDepth==maxDepth)
	{
		res.hval = m_utility->calculate(*(curNode.m_board),the_color);
		return res;
	}

	////////////////////////
	// Probabilistic state //
	////////////////////////
	if (state == SearchAlgorithm::PROB)
	{
		double L = m_utility->getLowerBound();
		double U = m_utility->getUpperBound();
		double updatedU = U, updatedL = L; // Updated bounds of the search

		if (false == BoardHelper::prepareSuccessorsProb(&probs,curNode))
		{
			res.move.m_square = SearchAlgorithm::SEARCH_TIMEOUT; //or size overflow in this case
			return res;
		}

		BoardNode * next = BoardHelper::nextSuccessor(curNode);
		while (next != NULL) {
			//((*iter)->m_hval) is the probability of this chance state !
			double prob = (next->m_hval);
			double cur_alpha = (alpha - updatedU + prob*U)/prob;
			double cur_beta = (beta - updatedL + prob*L)/prob;
			double ax = std::max(L,cur_alpha);
			double bx = std::min(U,cur_beta);
			// Run search on each son with updated bounds
			son_res = search(*next,currentDepth,maxDepth,SearchAlgorithm::DETER,timer,ax,bx);
			if (son_res.move.m_square == SearchAlgorithm::SEARCH_TIMEOUT) return son_res;

			updatedL += (son_res.hval - L)*prob;
			updatedU += (son_res.hval - U)*prob;
			if (son_res.hval >= cur_beta)
			{
				BoardHelper::endIteration(curNode);
				res.hval = beta;
				return res;
			}

			if (son_res.hval <= cur_alpha)
			{
				BoardHelper::endIteration(curNode);
				res.hval = alpha;
				return res;
			}
			next = BoardHelper::nextSuccessor(curNode);
		}

		assert((updatedU-updatedL)<0.0001);
		res.hval = updatedL;
		return res;
	}

	/////////////////////////
	// Deterministic state //
	/////////////////////////
	currentDepth++;

	// Find Max under alpha-beta bounds
	if (curNode.currPlayerColor == the_color)
	{
		if (false == BoardHelper::prepareSuccessorsNormal(m_lightUtil, curNode, the_color,false))
		{
			res.move.m_square = SearchAlgorithm::SEARCH_TIMEOUT; //or size overflow in this case
			return res;
		}

		res.hval = m_utility->getLowerBound();
		BoardNode* next = BoardHelper::nextSuccessor(curNode);
		while (next != NULL){
			son_res = search(*next, currentDepth, maxDepth, SearchAlgorithm::PROB, timer, alpha, beta);
			if (son_res.move.m_square == SearchAlgorithm::SEARCH_TIMEOUT) return son_res;

			if (son_res.hval > alpha)
			{
				if (son_res.hval >= beta)	// Maximum beyond bound, stop
				{
					BoardHelper::endIteration(curNode);
					res.hval = beta;
					return res;
				}
				alpha = son_res.hval;
				res.hval = alpha;
				res.move = next->m_move; // Found better Maximum bound
			}

			next = BoardHelper::nextSuccessor(curNode);
		}
	}
	else // curBoard.getCurPlayerColor() != the_color
	{
		if (false == BoardHelper::prepareSuccessorsNormal(m_lightUtil, curNode, the_color,true))
		{
			res.move.m_square = SearchAlgorithm::SEARCH_TIMEOUT; //or size overflow in this case
			return res;
		}

		res.hval = m_utility->getUpperBound();
		BoardNode* next = BoardHelper::nextSuccessor(curNode);
		while (next != NULL){
			son_res = search(*next,currentDepth,maxDepth,SearchAlgorithm::PROB,timer,alpha,beta);
			if (son_res.move.m_square == SearchAlgorithm::SEARCH_TIMEOUT) return son_res;

			if (son_res.hval < beta)
			{
				if (son_res.hval <= alpha)	// Minimum beyond bound, stop
				{
					BoardHelper::endIteration(curNode);
					res.hval = alpha;
					return res;
				}
				beta = son_res.hval;
				res.hval = beta;
				res.move = next->m_move; // Found better Minimum bound
			}

			next = BoardHelper::nextSuccessor(curNode);
		}
	}

	return res;
}


