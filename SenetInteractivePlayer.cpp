#include "Definitions.h"
#include "SenetInteractivePlayer.h"
#include "SenetChance.h"

SenetInteractivePlayer::SenetInteractivePlayer(Definitions::PlayerColor playerColor,
									 const GameTimer::TimeParams &tp, int moves_limit)
: SenetPlayer(playerColor, tp, moves_limit)
{
	setName("SenetInteractivePlayer");
}

SenetInteractivePlayer::~SenetInteractivePlayer()
{
}

void SenetInteractivePlayer::makeMove(const Board& curBoard, Move &myMove)
{
	SenetMove &km(dynamic_cast<SenetMove&>(myMove));
	const SenetBoard &kb(dynamic_cast<const SenetBoard&>(curBoard));
	
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
	vector<SenetMove> succ(kb.getSuccessors(m_myColor));
	for(unsigned int i(0); i < succ.size(); ++i)
		printf("%d ", succ[i].m_square);
	printf("\n");
	printf("Enter your move in the format of [square#] (without the brackets), ");
	printf("square# in the range of [0..%d], use -1 to indicate empty move: ", m_size_squares - 1);
	int user_square;
	fflush(stdout);
	scanf("%d", &user_square);
	fflush(stdin);
	km.m_square = user_square;
	printf("Move taken: %d\n", km.m_square);
	//int aaa; printf("#################################"); scanf("%d",&aaa); printf("##############################\n");

#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
	printf("Move: %s (%d)\n", Definitions::playerColorToString(m_myColor).c_str(), km.m_square);
#endif
}

void SenetInteractivePlayer::setName(const string &s)
{
	m_myName = s;
}
