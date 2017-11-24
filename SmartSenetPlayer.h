/* SmartSenetPlayer.h
 *
 * Concrete SenetPlayer for Senet game.
 *
 * First "thinking" automatic player
 * I use it as a basis to your implementation of the SenetPlayer.
 */

#ifndef __SmartSenetPlayer_H__
#define __SmartSenetPlayer_H__

// If defined, __AI_GAME_SMART1_PLAYER_DEBUG__ will cause the smart1 player
// to draw the board and show the available moves an the move taken
// (much like the interactive player screen)
//#define __AI_GAME_SMART1_PLAYER_DEBUG__

#include "SenetPlayer.h"

namespace senet_ai{
	class Utility;
	class SearchAlgorithm;
	struct BoardNode;
}

class SmartSenetPlayer : public SenetPlayer
{
public:
	// Constructor
	// Initializes player's color and game/move timer
	SmartSenetPlayer(Definitions::PlayerColor playerColor,
		const GameTimer::TimeParams &tp, int moves_limit);
	// Destructor
	// Does nothing
	~SmartSenetPlayer();
			   
	// Initializes the game.
	// Called at the beginning of the game session.
	// Initializes m_size_squares, m_num_pieces, m_num_sticks to given values,
	// and uses board information from initial_board.
	void initGame(int num_squares, int num_pieces,
					   int num_sticks, const SenetBoard &initial_board);

	// Reads the current board, requests a legal successors list,
	// places a smart1 legal successor in myMove
	void makeMove(const Board& curBoard, Move &myMove);
	// Sets m_myName to s
	void setName(const std::string &s);

	/** used to set-up this player's utility and search algorithm	 */
	void initSearch(senet_ai::Utility*, senet_ai::Utility*, senet_ai::SearchAlgorithm*,int);

#ifdef AMIT_STATS //used only in debug
	long movesMade;
	long totalDepth;
	double totalTime;
	long movesPerDepth[11];
	double timePerDepth[11];
#endif

protected:
	senet_ai::BoardNode *m_head;
	senet_ai::Utility *m_utility;

	senet_ai::Utility *m_light_utility;
	senet_ai::SearchAlgorithm *m_search_algorithm;
	int prev_total_size;

	//must be higher then 0
	int m_searchDepth;
};

#endif
