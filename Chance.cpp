#include "Chance.h"
#include <cstdlib>
using namespace std;

ChanceState::~ChanceState()
{}

Chance::Chance(unsigned int _seed)
{
	state = _seed;
	for(unsigned int i(0); i < _seed; ++i)
		NextInt(0, 1);
}

Chance::~Chance()
{}

int Chance::NextInt(int min, int max) const
{
	state = (1103515245 * state + 12345) & 0x7fffffffUL;
	double r = state / 2147483648.0;
	return (int)(r * (max - min) + min + 0.5);
}

ChanceState * Chance::getNextChanceState() const
{
	return NULL;
}
