#include "SenetMove.h"

SenetMove::SenetMove() : Move(), m_square(-1) 
{}

SenetMove::SenetMove(int c) : m_square(c) 
{}

SenetMove::~SenetMove() 
{}

bool SenetMove::isEmpty() const 
{
	return m_square == -1;
}
