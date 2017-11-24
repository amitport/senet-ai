#include "SenetPlayer.h"

SenetPlayer::SenetPlayer(Definitions::PlayerColor playerColor,
						 const GameTimer::TimeParams &tp, int moves_limit)
: Player(tp, moves_limit), m_myColor(playerColor)
{
}

SenetPlayer::~SenetPlayer()
{
}

void SenetPlayer::initGame(int num_squares, int num_pieces, 
					   int num_sticks, const SenetBoard &initial_board)
{
	m_size_squares = num_squares;
	m_num_pieces = num_pieces;
	m_num_sticks = num_sticks;
}
