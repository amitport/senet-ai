/* Chance.h
 *
 * An abstract chance class.
 */

#ifndef __CHANCE_H__
#define __CHANCE_H__

class ChanceState
{
public:
	virtual ~ChanceState();
};

class Chance
{
public:
	// Sets the random number generator to seed with <_seed>
	Chance(unsigned int _seed);

	// Virtual destructor
	virtual ~Chance();

	// Returns the next integer in range [min..max]
	int NextInt(int min, int max) const;

	// Gets the next chance state
	// Returns NULL unless overriden in a derived class
	// If not NULL, it is the caller's role to free this object's memory.
	virtual ChanceState * getNextChanceState() const;
	
private:
	mutable unsigned long state;
};

#endif
