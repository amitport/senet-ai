/* UtilitySmart1.h
 *
 */

#ifndef __UTILITY_Smart1_H__
#define __UTILITY_Smart1_H__

#include "Utility.h"

class UtilitySmart1 : public Utility
{
public:
	// Constructor
	UtilitySmart1();
	// Destructor
	// Does nothing
	virtual ~UtilitySmart1() {};
	
	// Reads the current board, get a current move,
	// Calculate utility function for suggested board
	double calculate(const SenetBoard& curBoard, Definitions::PlayerColor playerColor);
	void initialize(int size_squares, int num_pieces);
};

#endif
