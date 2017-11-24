#include "Player.h"

Player::Player(const GameTimer::TimeParams &t, int moves) : 
m_gameTimerParams(t), m_moves_limit(moves)
{
}

Player::~Player()
{
}

string Player::getName() const
{
	return m_myName;
}
