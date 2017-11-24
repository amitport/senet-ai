#include "GameTimer.h"
#include <limits>

using std::numeric_limits;

// Allowed overhead for time checks.
// Do not rely on this number (i.e. set to 0 or near 0)
// when checking time limits.
const double allowedOverheadTime(0.03);

GameTimer::GameTimer(const TimeParams &timeParams) :
m_timeParams(timeParams)
{
}

GameTimer::GameTimer(const GameTimer &gameTimer)
{
	m_timeParams = gameTimer.m_timeParams;
	m_gameStartTime = gameTimer.m_gameStartTime;
	m_moveStartTime = gameTimer.m_moveStartTime;
}

GameTimer::~GameTimer()
{
}

#ifdef WIN32

void GameTimer::getCurTime(clock_t &curTime) const
{
	curTime = clock();
}

double GameTimer::timeDiffInSec(clock_t before, clock_t after) const 
{
	return (after-before)/(double)CLOCKS_PER_SEC;
}

#else

void GameTimer::getCurTime(struct tms &tv) const
{
	times(&tv);
}

double GameTimer::timeDiffInSec(const struct tms &before, 
				const struct tms &after) const
{
	return (after.tms_utime - before.tms_utime)/(double)HZ;
}

#endif

void GameTimer::startGameTimer()
{
	getCurTime(m_gameStartTime);
}

bool GameTimer::isGameTimePassed(GameTimer::OverheadType useOverhead) const
{
	return getRemainingGameTime() + (useOverhead == USE_OVERHEAD ? allowedOverheadTime : 0) < 0;
}

double GameTimer::getRemainingGameTime() const
{
	if(!m_timeParams.timePerGame_limit)
		return numeric_limits<double>::max();

#ifdef WIN32
	clock_t curTime;
#else
	struct tms curTime;
#endif
	getCurTime(curTime);
	return (m_timeParams.timePerGame - timeDiffInSec(m_gameStartTime, curTime));
}

void GameTimer::startMoveTimer()
{
	getCurTime(m_moveStartTime);
}

bool GameTimer::isMoveTimePassed(GameTimer::OverheadType useOverhead) const
{
	return getRemainingMoveTime() + (useOverhead == USE_OVERHEAD ? allowedOverheadTime : 0) < 0 ;
}

double GameTimer::getRemainingMoveTime() const
{
	if(!m_timeParams.timePerMove_limit)
		return numeric_limits<double>::max();

#ifdef WIN32
	clock_t curTime;
#else
	struct tms curTime;
#endif
	getCurTime(curTime);
	return (m_timeParams.timePerMove - timeDiffInSec(m_moveStartTime, curTime));
}
