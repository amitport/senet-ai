/*
 * BoardHelper.h
 *
 *  Created on: Dec 17, 2009
 *      Author: amitport
 */

#ifndef BoardHelper_H_
#define BoardHelper_H_

#include "SenetBoard.h"
#include <vector>
#include "Utility.h"

namespace senet_ai{

	struct BoardNode;

	/**
	 * contains static function that control
	 * simple board optimization
	 * successors cache
	 * and help keeping statistics
	 */
	class BoardHelper{
	public:
		static void init(int size_squares,int node_size);

		//check if the next move is legal
		static bool isLegalMove(const int c,const SenetBoard& curBoard,Definitions::PlayerColor color);
		//get the next legal moves
		static std::vector<SenetMove>  getNextMoves(const SenetBoard& curBoard,Definitions::PlayerColor color);

		//prepare the next nodes of a normal search node (see SearchAlgorithm, BoardNode)
		static bool prepareSuccessorsNormal(Utility *utility, BoardNode &curNode,Definitions::PlayerColor color, bool ascending);
		//prepare the next nodes of a probabilistic search node (see  SearchAlgorithm, BoardNode)
		static bool prepareSuccessorsProb(std::vector<double>* prob, BoardNode &curNode);

		//get the next successor in curNode iteration (must call prepare Successor on curNode first)
		static BoardNode* nextSuccessor(BoardNode &curNode);
		//end the current iteration on curNode (unnecessary if nextSuccessor returned false)
		static void endIteration(BoardNode &curNode);

		//those are mainly for statistics
		static int my_size_squares;
		static int my_total_size;
		static int my_node_size;
		static int max_size_threshold;
		static int warning_size_threshold;
		static bool doSort;
	};

	/**
	 * keeps data related to current node in our search tree (see SearchAlgorithm)
	 */
	struct BoardNode {
	public:
		//taking minimal information from the board (we may delete it after)
		void updateBoardFields(){
			currPlayerColor = m_board->getCurPlayerColor();
			boardResult = m_board->getBoardResult();
			sticksState = m_board->getSticksState();
		};

		//keep track of tree size when constructing and destructing nodes
		BoardNode(): m_board(NULL),m_succ(NULL),my_index(0),noSpace(false)
				{
				BoardHelper::my_total_size += BoardHelper::my_node_size;
				};
		BoardNode(SenetBoard* board, SenetMove move, double hval)
			: m_board(board),m_move(move),m_hval(hval),m_succ(NULL),my_index(0),noSpace(false)
			  {
				BoardHelper::my_total_size += BoardHelper::my_node_size;
				updateBoardFields();
			  };
		~BoardNode() {
			if (m_board != NULL) {
				delete m_board;
			}
			BoardHelper::my_total_size -= BoardHelper::my_node_size;

			if (m_succ != NULL) {
				for (std::vector<BoardNode*>::iterator iter = m_succ->begin(); iter!=m_succ->end(); ++iter )
				{
					delete (*iter);
				}
				delete m_succ;
			}
		}


		SenetBoard* m_board; //pointer the the board in this node
		Definitions::PlayerColor currPlayerColor;
		SenetBoard::BoardResult boardResult;
		int sticksState;
		SenetMove m_move; // the move the got as to this board
		double m_hval; //the heuristic value this node got
		std::vector<BoardNode*>* m_succ; // the list of successors of the node
		int my_index; //holds the current index in successors iteration (see BoardHelper::nextSuccessor)
		bool noSpace; //a flag indicating that from this node on we do not want to save anything we don't need

		// the function that is used to compare the boards for sorting in descending order
		static bool comparator (BoardNode *i,BoardNode *j) { return (i->m_hval > j->m_hval);}
		static bool ascendingComparator (BoardNode *i,BoardNode *j) { return (i->m_hval < j->m_hval);}
	};
}

#endif /* BoardHelper_H_ */
