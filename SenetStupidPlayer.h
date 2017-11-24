/* SenetStupidPlayer.h
 *
 * Concrete SenetPlayer for Senet game.
 *
 * This is a demo class and not an integral part of the game system.
 * You can use it as a basis to your implementation of the SenetPlayer.
 */

#ifndef __Stupid_Senet_PLAYER_H__
#define __Stupid_Senet_PLAYER_H__

// If defined, __AI_GAME_STUPID_PLAYER_DEBUG__ will cause the stupid player
// to draw the board and show the available moves an the move taken
// (much like the interactive player screen)
//#define __AI_GAME_STUPID_PLAYER_DEBUG__

#include "SenetPlayer.h"

class SenetStupidPlayer : public SenetPlayer
{
public:
	// Constructor
	// Initializes player's color and game/move timer
	SenetStupidPlayer(Definitions::PlayerColor playerColor,
		const GameTimer::TimeParams &tp, int moves_limit);
	// Destructor
	// Does nothing
	~SenetStupidPlayer();

	// Reads the current board, requests a legal successors list,
	// places a stupid legal successor in myMove
	void makeMove(const Board& curBoard, Move &myMove);
	// Sets m_myName to s
	void setName(const string &s);

};

#endif
