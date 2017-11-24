/*
 * BoardHelper.cpp
 *
 *  Created on: Dec 20, 2009
 *      Author: amitport
 */

#include "BoardHelper.h"

using namespace senet_ai;
using std::vector;

int BoardHelper::my_size_squares;
int BoardHelper::my_total_size = 0;
int BoardHelper::my_node_size = 1000;


int BoardHelper::max_size_threshold = 90000000;
int BoardHelper::warning_size_threshold = 80000000;
bool BoardHelper::doSort = true;

void BoardHelper::init(int size_squares, int node_size)
{
	BoardHelper::my_size_squares = size_squares;
	BoardHelper::my_node_size = node_size;
}

bool BoardHelper::isLegalMove(const int c,const SenetBoard& curBoard,Definitions::PlayerColor color)
{
	SenetBoard::SquareData sd = curBoard.getBoardState(c);
	Definitions::PlayerColor oppColor = Definitions::getOppositePlayer(color);

	//not my square to move
	if (sd.player_color != color)
		return false;

	int move_to_square = c + curBoard.getSticksState();

	//check if we can exit
	if(move_to_square >= my_size_squares){
		int firstThird = my_size_squares / 3;
		for(int i(0); i < firstThird; ++i)
			if(curBoard.getBoardState(i).player_color == color)
			{
				return false;
			}
		//we still need to check for bunker
	}else{
		//can't move on my own player
		if (curBoard.getBoardState(move_to_square).player_color == color)
			return false;
		//if moving on opponent we need to check he's not safe
		if (curBoard.getBoardState(move_to_square).player_color == oppColor){
			if (//opp on safe square
				(curBoard.getBoardState(move_to_square).square_type == SenetBoard::SAFE)
					||
				//opp is on safe seq (from behind)
				(curBoard.getBoardState(move_to_square-1).player_color == oppColor)
					||
				//opp is on safe seq (from front)
				((move_to_square + 1 < my_size_squares) &&
					(curBoard.getBoardState(move_to_square+1).player_color == oppColor))
				){
				return false;
			}
		}
	}

	//check bunker
	if (curBoard.getSticksState() >= 4){ //no point of checking if we can't move over it
		int oppSeqCount = 0;
		int upTo = std::min(move_to_square,my_size_squares);
		for(int i(c+1); i<upTo;i++){
			if (curBoard.getBoardState(i).player_color == oppColor){
				oppSeqCount++;
			}else{
				oppSeqCount = 0;
			}
			if (oppSeqCount == 3){
				return false;
			}
		}
	}

	return true;
}

vector<SenetMove>  BoardHelper::getNextMoves(const SenetBoard& curBoard,Definitions::PlayerColor color){
	vector<SenetMove> succ;

	if (0 != curBoard.getSticksState()){
		for(int c(0); c < my_size_squares; ++c)
		{
			if(isLegalMove(c,curBoard,color)){
				SenetMove move(c);
				succ.push_back(move);
			}
		}
	}

	if (succ.empty()){
		SenetMove emptyMove;
		succ.push_back(emptyMove);
	}
	return succ;
}

/**
 * just receive a node and create its successor node (normal)
 */
static void populateNormalSuccessors(vector<BoardNode*>* succ, Utility *utility, BoardNode &curNode,Definitions::PlayerColor color){
	vector<SenetMove>  moves = BoardHelper::getNextMoves(*(curNode.m_board),curNode.m_board->getCurPlayerColor());
	for(size_t i = 0 ; i < moves.size(); i++){
		SenetBoard * kbcopy = new SenetBoard(*(curNode.m_board));
		kbcopy->makeMove(kbcopy->getCurPlayerColor(),moves[i]);
		succ->push_back(new BoardNode(kbcopy,moves[i],utility->calculate(*kbcopy,color)));
	}
}

/**
 * just receive a node and create its successor node (probabilistic)
 */
static void populateProbSuccessors(vector<BoardNode*>* succ, vector<double>* prob, BoardNode &curNode)
{
	for(int i = 0 ; i < (int)prob->size(); i++){
		BoardNode* t = new BoardNode();

		SenetBoard * kbcopy = new SenetBoard(*(curNode.m_board));
		SenetChanceState chance_state(i);
		kbcopy->setChanceState(chance_state);
		t->m_board = kbcopy;

		t->updateBoardFields();

		t->m_hval = (*prob)[i];
		t->m_move = SenetMove(i);
		succ->push_back(t);
	}
}

/**
 * Creates the successors and decide whether this node will be saving information to cache or will be disposable
 */
static bool makeSuccessors(BoardNode &curNode, vector<double>* prob, Utility *utility, Definitions::PlayerColor color, bool ascending){
	curNode.my_index = 0;

	if (curNode.m_succ != NULL){
		//the successors of this node are cached !
		return true;
	}


	if (BoardHelper::my_total_size > BoardHelper::warning_size_threshold){	//we are using more then 80 MB -> time to stop
		//at this point we wont use any more memory then needed
		curNode.noSpace = true;
		if (BoardHelper::my_total_size > BoardHelper::max_size_threshold){
			//at this point we will stop the search
			return false;
		}
	}

	vector<BoardNode*>* succ = new vector<BoardNode*>();

	//create the node's successors
	if (prob != NULL){
		populateProbSuccessors(succ, prob, curNode);
	} else {
		populateNormalSuccessors(succ, utility, curNode, color);
	}



	if (BoardHelper::doSort) //only used in experiments
	{
		//sort the successors according to the 'ascending' flag
		std::sort(succ->begin(),succ->end(),(ascending)?
											 BoardNode::ascendingComparator:
											 BoardNode::comparator);
	}


	curNode.m_succ = succ;
	if (!curNode.noSpace)
	{
		//if we are saving the successors to cache we do not need the save the board
		delete curNode.m_board;
		curNode.m_board = NULL;
	}

	return true;
}

bool BoardHelper::prepareSuccessorsNormal(Utility *utility, BoardNode &curNode,Definitions::PlayerColor color, bool ascending){
	return makeSuccessors(curNode,NULL,utility,color,ascending);
}

//return true if everything is OK
//false if we run out of memory (the higher bound)
bool BoardHelper::prepareSuccessorsProb(vector<double>* prob, BoardNode &curNode){
	return makeSuccessors(curNode,prob,NULL,Definitions::NONE,false);
}

BoardNode* BoardHelper::nextSuccessor(BoardNode &curNode){
	if (curNode.noSpace && curNode.my_index > 0){
		//this is not the first son and we got no space -> delete the previous one
		delete (*(curNode.m_succ))[curNode.my_index-1];
		(*(curNode.m_succ))[curNode.my_index-1] = NULL;
	}

	if (curNode.my_index == (int)(*(curNode.m_succ)).size()){
		//we got no more successors
		if (curNode.noSpace){
			//we got no space -> we didn't delete the board before -> delete it now
			delete curNode.m_succ;
			curNode.m_succ = NULL;
		}
		return NULL;
	}

	BoardNode * next = (*(curNode.m_succ))[curNode.my_index];

	curNode.my_index++;
	//return the son with a board intact
	return next;
}

void BoardHelper::endIteration(BoardNode &curNode){
	if (curNode.noSpace){
		//we got no space -> delete the successors
		for(;curNode.my_index <= (int)(*(curNode.m_succ)).size();curNode.my_index++){
			delete (*(curNode.m_succ))[curNode.my_index-1];
			(*(curNode.m_succ))[curNode.my_index-1] = NULL;
		}

		delete curNode.m_succ;
		curNode.m_succ = NULL;
	}
}
