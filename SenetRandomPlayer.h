/* SenetRandomPlayer.h
 *
 * Concrete SenetPlayer for Senet game.
 *
 * This is a demo class and not an integral part of the game system.
 * You can use it as a basis to your implementation of the SenetPlayer.
 */

#ifndef __Random_Senet_PLAYER_H__
#define __Random_Senet_PLAYER_H__

// If defined, __AI_GAME_RANDOM_PLAYER_DEBUG__ will cause the random player
// to draw the board and show the available moves an the move taken
// (much like the interactive player screen)
//#define __AI_GAME_RANDOM_PLAYER_DEBUG__

#include "SenetPlayer.h"

class SenetRandomPlayer : public SenetPlayer
{
public:
	// Constructor
	// Initializes player's color and game/move timer
	SenetRandomPlayer(Definitions::PlayerColor playerColor,
		const GameTimer::TimeParams &tp, int moves_limit);
	// Destructor
	// Does nothing
	~SenetRandomPlayer();

	// Reads the current board, requests a legal successors list,
	// places a random legal successor in myMove
	void makeMove(const Board& curBoard, Move &myMove);
	// Sets m_myName to s
	void setName(const string &s);

private:
	// Returns a random integer in the range [1..range]
	int RandInt(int range);
};

#endif
