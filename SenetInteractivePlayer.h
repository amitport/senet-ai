/* SenetRandomPlayer.h
 *
 * Concrete SenetPlayer for Senet game.
 *
 * This is a demo class and not an integral part of the game system.
 * You can use it as an aid to your implementation of the SenetPlayer.
 */

#ifndef __Interactive_Senet_PLAYER_H__
#define __Interactive_Senet_PLAYER_H__

#include "SenetPlayer.h"

class SenetInteractivePlayer : public SenetPlayer
{
public:
	// Constructor
	// Initializes player's color and game/move timer
	SenetInteractivePlayer(Definitions::PlayerColor playerColor,
		const GameTimer::TimeParams &tp, int moves_limit);
	// Destructor
	// Does nothing
	~SenetInteractivePlayer();

	// Reads the current board, prints it on screen,
	// requests a move from the user and places it in myMove
	void makeMove(const Board& curBoard, Move &myMove);
	// Sets m_myName to s
	void setName(const string &s);
};

#endif
