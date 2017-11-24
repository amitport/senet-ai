/* Board.h
 *
 * An abstract game board class.
 *
 */

#ifndef __BOARD_H__
#define __BOARD_H__

#include "Definitions.h"
#include "Move.h"
#include "Chance.h"
#include <vector>
#include <string>

using std::vector;
using std::string;

class Board
{
public:
	// Virtual Destructor
	virtual ~Board() {};

	// Returns true iff player p can make move m in the current board state
	virtual bool isLegalMove(const Definitions::PlayerColor p, const Move& m) const = 0;
	
	// Updates the current board state with move m of player p.
	// Not guarranteed to check if the move is legal.
	virtual void makeMove(const Definitions::PlayerColor p, const Move& m) = 0;

	// Updates the current chance state
	virtual void setChanceState(const ChanceState &chance_state) = 0;

	// Outputs the current board state to stdout.
	// playersNames is a vector of players' names, ordered according to
	// their order in the game class.
	virtual void drawBoard(const vector<string> &playersNames) const = 0;

protected:
	ChanceState *m_chance_state;	// The current chance state
};

#endif
