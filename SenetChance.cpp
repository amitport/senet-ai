#include "SenetChance.h"

SenetChance::SenetChance(int num_sticks, unsigned int _seed)
: Chance(_seed), m_num_sticks(num_sticks)
{}

SenetChance::~SenetChance()
{}

ChanceState * SenetChance::getNextChanceState() const
{
	int num_ON_sticks(0);
	for(int i(0); i < m_num_sticks; ++i)
		num_ON_sticks += NextInt(0, 1);
	return new SenetChanceState(num_ON_sticks);
}

SenetChanceState::SenetChanceState() : num_ON_sticks(-1)
{}

SenetChanceState::SenetChanceState(int ON_sticks) : num_ON_sticks(ON_sticks)
{}

SenetChanceState::SenetChanceState(const ChanceState * scs)
{
	num_ON_sticks = (dynamic_cast<const SenetChanceState*>(scs))->num_ON_sticks;
}

SenetChanceState::~SenetChanceState()
{}

