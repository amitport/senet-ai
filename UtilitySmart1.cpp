/* UtilitySmart1.cpp
 *
 */

#include "UtilitySmart1.h"

UtilitySmart1::UtilitySmart1() {}

/** Reads the current board, get a current move,
 * Calculate utility function for suggested board */
double UtilitySmart1::calculate(const SenetBoard& curBoard, Definitions::PlayerColor playerColor)
{
	double val = 0.0;
    int i;
    for (i=0; i<m_size_squares; i++)
    {
    	if (playerColor == curBoard.getBoardState(i).player_color)
    		val += m_size_squares - i;
        else if (Definitions::getOppositePlayer(playerColor) == curBoard.getBoardState(i).player_color)
            val -= m_size_squares - i;
    }
    return -val;
}

void UtilitySmart1::initialize(int size_squares, int num_pieces)
{
	setParams(size_squares,num_pieces);
	m_upperBound = 0;
	for (int i=0 ; i<num_pieces ; i++)
	{
		m_upperBound += size_squares-i;
	}
	m_lowerBound = -m_upperBound;
}
