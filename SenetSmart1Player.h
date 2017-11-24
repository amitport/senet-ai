/* SenetSmart1Player.h
 *
 * Concrete SenetPlayer for Senet game.
 *
 * First "thinking" automatic player
 * I use it as a basis to your implementation of the SenetPlayer.
 */

#ifndef __Smart1_Senet_PLAYER_H__
#define __Smart1_Senet_PLAYER_H__

// If defined, __AI_GAME_SMART1_PLAYER_DEBUG__ will cause the smart1 player
// to draw the board and show the available moves an the move taken
// (much like the interactive player screen)
//#define __AI_GAME_SMART1_PLAYER_DEBUG__

#include "SenetPlayer.h"
#include "UtilitySmart1.h"
#include "Expectimax.h"
#include "Star1.h"

class SenetSmart1Player : public SenetPlayer
{
public:
	// Constructor
	// Initializes player's color and game/move timer
	SenetSmart1Player(Definitions::PlayerColor playerColor,
		const GameTimer::TimeParams &tp, int moves_limit);
	// Destructor
	// Does nothing
	~SenetSmart1Player();
			   
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
	void setName(const string &s);

	/** used to set-up this player's utility and search algorithm	 */
	void initSearch(Utility*,SearchAlgorithm*);

protected:
	BoardNode *m_head;
	Utility *m_utility;
	SearchAlgorithm *m_search_algorithm;

	//must be higher then 0
	static const int SEARCH_DEPTH;
};

#endif
