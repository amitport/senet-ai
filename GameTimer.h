/* GameTimer.h
 *
 * Game and move timer.
 * Provides methods to start and check given time limits.
 *
 * Supports both Windows and Linux.
 */

#ifndef __GAME_TIMER_H__
#define __GAME_TIMER_H__

#ifdef WIN32
#include <time.h>
#else
#include <sys/times.h>
#include <sys/param.h>
#include <sys/types.h>
#endif

class GameTimer
{
public:
	// Time parameters structure
	typedef struct
	{
		bool timePerMove_limit;		// Is move time limited
		double timePerMove;			// Time limit per move, in seconds
		bool timePerGame_limit;		// Is game time limited
		double timePerGame;			// Time limit per game, in seconds
	} TimeParams;
	
	// Overhead enum
	typedef enum {USE_OVERHEAD, NO_OVERHEAD} OverheadType;

	// Constructor
	// Copies timeParams data to m_timeParams
	// Does not start timers
	GameTimer(const TimeParams &timeParams);
	// Copy Constructor
	// Copies gameTimer time parameters and state of timers
	GameTimer(const GameTimer &gameTimer);
	// Virtual Destructor
	~GameTimer();

	// Starts the game timer and records this time
	void startGameTimer();
	// Has the game time passed
	// If <useOverhead> is true, include some grace time in the calculation
	bool isGameTimePassed(OverheadType useOverhead) const;
	// Returns allowed game time minus (current time minus recorded game time start)
	// in seconds
	double getRemainingGameTime() const;

	// Starts the move timer and records this time
	void startMoveTimer();
	// Has the move time passed
	// If <useOverhead> is true, include some grace time in the calculation
	bool isMoveTimePassed(OverheadType useOverhead) const;
	// Returns allowed move time minus (current time minus recorded move time start)
	// in seconds
	double getRemainingMoveTime() const;

private:
#ifdef WIN32
	// Gets current time in clock_t units
	void getCurTime(clock_t &curTime) const;
	// Returns time difference in seconds of (after - before)
	double timeDiffInSec(clock_t before, clock_t after) const;
#else
	// Gets current time in clock_t units
	void getCurTime(struct tms &tv) const;
	// Returns time difference in seconds of (after - before)
	double timeDiffInSec(const struct tms &before, 
		const struct tms &after) const;
#endif

	TimeParams m_timeParams;	// Timer parameters
#ifdef WIN32
	clock_t m_gameStartTime;	// Game start time in clock_t units
	clock_t m_moveStartTime;	// Move start time in clock_t units
#else
	struct tms m_gameStartTime;	// Game start time in tms units
	struct tms m_moveStartTime;	// Move start time in tms units
#endif
};

#endif
