/* SenetMove.h
 *
 * Concrete Move class for Senet game.
 */
#ifndef __SENET_MOVE_H__
#define __SENET_MOVE_H__

#include "Move.h"

class SenetMove : public Move
{
public:
	// Constructor
	// Empty constructor, assign invalid move
	SenetMove();
	// Constructor
	// Assign move c to m_column, a to m_action, no checks
	SenetMove(int c);
	// Destructor
	~SenetMove();

	// Returns true iff the move is the default, empty move
	bool isEmpty() const;

	int m_square;			// Player's move column
};

#endif
