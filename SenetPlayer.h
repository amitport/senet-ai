/* SenetPlayer.h
 *
 * Abstract class for the Senet game.
 */

#ifndef __SENET_PLAYER_H__
#define __SENET_PLAYER_H__

#include "Player.h"
#include "SenetMove.h"
#include "SenetBoard.h"
#include "GameTimer.h"

class SenetPlayer : public Player
{
public:
	// Constructor
	// Initializes player color m_myColor and game/move time parameters
	// which can be accessed through m_gameTimer (inherited from class Player)
	SenetPlayer(Definitions::PlayerColor playerColor, 
		const GameTimer::TimeParams &tp, int moves_limit);
	// Destructor
	virtual ~SenetPlayer();

	// Initializes the game.
	// Called at the beginning of the game session.
	// Initializes m_size_squares, m_num_pieces, m_num_sticks to given values,
	// and uses board information from initial_board.
	virtual void initGame(int num_squares, int num_pieces, 
					   int num_sticks, const SenetBoard &initial_board);

	// Computes a move based on the given board state.
	// Should cast myMove to SenetMove and store the move column in
	// range [1..m_size_cols]
	virtual void makeMove(const Board& curBoard, Move &myMove) = 0;

	// Sets m_myName to s
	virtual void setName(const string &s) = 0;

protected:
	int m_size_squares;			// The number of board squares
	int m_num_pieces;			// The number of pieces of each player
								//	at the beginning of the game
	int m_num_sticks;			// The number of sticks
	Definitions::PlayerColor m_myColor;	// This player's color
};

#endif
