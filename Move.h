/* Move.h
 *
 * Abstract class for a move in a game.
 */

#ifndef __MOVE_H__
#define __MOVE_H__

#include "Definitions.h"

class Move
{
public:
	// Destructor
	virtual ~Move() {};

	// Returns true iff the move is the default, empty move
	virtual bool isEmpty() const = 0;
};

#endif
