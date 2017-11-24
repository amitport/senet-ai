/* Utility.cpp
 *
 */

#include "Utility.h"

using namespace senet_ai;

void Utility::setParams(int size_squares, int num_pieces, int num_sticks)
{
	m_size_squares = size_squares;
	m_num_pieces = num_pieces;
	m_num_sticks = num_sticks;
}

void Utility::initialize(int size_squares, int num_pieces, int num_sticks)
{
	setParams(size_squares,num_pieces,num_sticks);
}

double Utility::getLowerBound()
{
	return m_lowerBound;
}

double Utility::getUpperBound()
{
	return m_upperBound;
}
