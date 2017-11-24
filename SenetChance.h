/* SenetChance.h
 *
 * A concrete Chance class for the Senet game.
 */

#ifndef __SENET_CHANCE_H__
#define __SENET_CHANCE_H__

#include "Chance.h"

class SenetChanceState : public ChanceState
{
public:
	// Default constructor - sets num_ON_sticks to -1
	SenetChanceState();
	// Constructor - sets num_ON_sticks to <ON_sticks>
	SenetChanceState(int ON_sticks);
	// Constructor - Assumes <scs> is a SenetChanceState and copies its num_ON_sticks
	SenetChanceState(const ChanceState * scs);
	
	// Destructor
	~SenetChanceState();

	int num_ON_sticks;	// Number of sticks that are ON
};

class SenetChance : public Chance
{
public:
	// Constructor - sets m_num_sticks to <num_sticks>
	// and initializes random number generator with seed <_seed>
	SenetChance(int num_sticks, unsigned int _seed);

	// Destructor
	~SenetChance();

	// Throws the sticks to the air
	// The ones landing on the ground with their 1 side up
	// are counted as ON sticks.
	// Returns a new SenetChanceState with num_ON_sticks set to this amount.
	// It is the caller's role to free this object's memory.
	ChanceState * getNextChanceState() const;

private:
	int m_num_sticks;	// The number of sticks to throw
};

#endif
