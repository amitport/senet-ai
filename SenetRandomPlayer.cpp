#include "Definitions.h"
#include "SenetRandomPlayer.h"

SenetRandomPlayer::SenetRandomPlayer(Definitions::PlayerColor playerColor,
									 const GameTimer::TimeParams &tp, int moves_limit)
: SenetPlayer(playerColor, tp, moves_limit)
{
	setName("RandomSenetPlayer");
}

SenetRandomPlayer::~SenetRandomPlayer()
{
}

int SenetRandomPlayer::RandInt(int range)
{
	return (int)((rand()/(double)RAND_MAX) * (range-1) + 1.5);
}

void SenetRandomPlayer::makeMove(const Board& curBoard, Move &myMove)
{
	// Do the following each time you start makeMove or initGame
	GameTimer my_timer(m_gameTimerParams);
	my_timer.startMoveTimer();

	SenetMove &km(dynamic_cast<SenetMove&>(myMove));
	const SenetBoard &kb(dynamic_cast<const SenetBoard&>(curBoard));

	vector<SenetMove> succ(kb.getSuccessors(m_myColor));
	km = succ[RandInt(succ.size())-1];

	// The following is an example, uncomment it to see that
	// overhead can be a life saver
	/*
	// Start of example
	// Use only isMoveTimePassed(GameTimer::NO_OVERHEAD) to check for move/init time
	while(!my_timer.isMoveTimePassed(GameTimer::NO_OVERHEAD));
	// The game system uses isMoveTimePassed(GameTimer::USE_OVERHEAD) to check for move/init time
	if(!my_timer.isMoveTimePassed(GameTimer::USE_OVERHEAD))
		printf("PHEW I canz scape mai recurshunz now");
	else
		printf("OMG WTF u gives me no timez to tink");
	// End of example 
	*/

#ifdef __AI_GAME_RANDOM_PLAYER_DEBUG__
	vector<string> players_names;
	if(m_myColor == Definitions::WHITE)
	{
		players_names.push_back(getName());
		players_names.push_back("OPPONENT");
	}
	else
	{
		players_names.push_back("OPPONENT");
		players_names.push_back(getName());
	}

	kb.drawBoard(players_names);

	printf("%s ", getName().c_str());
	printf("[%s (%c)] ", Definitions::playerColorToString(m_myColor).c_str(), Definitions::playerColorToChar(m_myColor));
	printf("to move.\n");
	printf("Current sticks state: %d\n", kb.getSticksState());
	printf("Legal moves: ");
	for(unsigned int i(0); i < succ.size(); ++i)
		printf("%d ", succ[i].m_square);
	printf("\n");

	printf("Move taken: %d\n", km.m_square);
#endif

#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
	printf("Move: %s (%d)\n", Definitions::playerColorToString(m_myColor).c_str(), km.m_square);
#endif
	fflush(stdout);
}

void SenetRandomPlayer::setName(const string &s)
{
	m_myName = s;
}
