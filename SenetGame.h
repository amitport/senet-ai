/* SenetGame.h
 *
 * Concrete Game class for the Senet game.
 */

#ifndef __SENET_GAME_H__
#define __SENET_GAME_H__

#include <time.h>
#include <vector>
#include "Definitions.h"
#include "SenetBoard.h"
#include "SenetMove.h"
#include "SenetPlayer.h"
#include "Game.h"

using std::vector;

class SenetGame : public Game
{
public:
	// Constructor
	// Inserts players into m_Players (from ::Game)
	// (player 0 is WHITE, player 1 is BLACK)
	// Records game and move parameters timeParams
	// Initializes game board with parameters num_squares, num_pieces, 
	// num_extra_safe, num_extra_rebirth, num_sticks
	SenetGame(vector<Player*> players, double player_initGame_time_limit,
		GameTimer::TimeParams timeParams, int num_squares, int num_pieces, 
		int num_extra_safe, int num_extra_rebirth,
		int num_sticks, unsigned int chance_seed, int max_moves);
	// Destructor
	~SenetGame();

	// Initializes players (using their own initGame) and checks for
	// proper initialization time (of m_allowedPlayerInitializationTime)
	void initGame();
	// Makes move m of current player m_curPlayer
	// Uses SenetBoard::makeMove, does not check for validity of the move
	void makeMove(const Move &m);
	// Returns true iff the current turn is finished (that is -
	// no subsequent moves in this turn)
	bool isTurnFinished() const;
	// Sets the next chance state to the board
	void setNextChanceState();
	// Gets the position of the current player in the players' vector
	int getCurPlayer() const;
	// Gets the current player's color
	Definitions::PlayerColor getCurPlayerColor() const;
	// Get the game result of player player_num, as reported by
	// SenetBoard::getBoardResult
	GameRes::ResType getPlayerResult(int player_num) const;
	// Returns an empty initialized SenetMove
	// It is the caller's role to free this object's memory.
	Move *getEmptyMove() const;
	// Returns a copy of the current board state
	// It is the caller's role to free this object's memory.
	Board *getCurBoardCopy() const;
	// Outputs the current board state to the console
	void drawBoard() const;

protected:
	// Returns true iff the board state is final
	// i.e. no stones in any of the players' house rows
	bool isFinalState() const;
	// Sets game result from and into Game::m_gameRes.players_result
	bool calcGameResult();
	// Returns true iff the game is based on chance
	bool isChanceGame() const;

	int m_size_squares;			// The number of board squares
	int m_num_pieces;			// The number of pieces of each player
								//	at the beginning of the game
	int m_num_extra_safe;		// The number of extra safe squares
	int m_num_extra_rebirth;	// The number of extra rebirth squares
	int m_num_sticks;			// The number of sticks

	const int m_WhiteNum;		// White player position in m_Players
	const int m_BlackNum;		// Black player position in m_Players
};

#endif
