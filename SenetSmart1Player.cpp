#include "Definitions.h"
#include "SenetSmart1Player.h"
#include "BoardHelper.h"

//#define __AI_GAME_SMART1_PLAYER_DEBUG__

SenetSmart1Player::SenetSmart1Player(Definitions::PlayerColor playerColor,
									 const GameTimer::TimeParams &tp, int moves_limit)
: SenetPlayer(playerColor, tp, moves_limit)
{
	m_utility = NULL;
	m_search_algorithm = NULL;
	m_head = NULL;
}

SenetSmart1Player::~SenetSmart1Player()
{
	if (m_utility != NULL)
		delete m_utility;
	if (m_search_algorithm != NULL)
		delete m_search_algorithm;
	if (m_head != NULL)
		delete m_head;
}

void SenetSmart1Player::initGame(int num_squares, int num_pieces,
					   int num_sticks, const SenetBoard &initial_board)
{
//	GameTimer::TimeParams initTp;
//	initTp.timePerGame_limit = false;
//	initTp.timePerMove_limit = true;
//	initTp.timePerMove = 10; //hard-coded since it's not a parameter
//	GameTimer initGt(initTp);

	setName("Smart1SenetPlayer");
	m_size_squares = num_squares;
	m_num_pieces = num_pieces;
	m_num_sticks = num_sticks;
	BoardHelper::init(m_size_squares);
	initSearch(new UtilitySmart1(),new Expectimax());

	//if (initGt.isMoveTimePassed(GameTimer::NO_OVERHEAD)){
	//	exit now !
	//}
}

void SenetSmart1Player::makeMove(const Board& curBoard, Move &myMove)
{
	GameTimer my_timer(m_gameTimerParams);
	my_timer.startMoveTimer();

	SenetMove &km(dynamic_cast<SenetMove&>(myMove));

	SearchResult bestRes;
	if (NULL != m_head){
//		printf("nodes = %d size per node = %d => freeing %d bytes\n",
//				BoardHelper::my_num_nodes,
//				sizeof(SenetBoard::SquareData)* m_size_squares, //just an estimate
//				BoardHelper::my_num_nodes * sizeof(SenetBoard::SquareData)*m_size_squares);

		BoardHelper::my_num_nodes = 1;
		delete m_head;
	}
	
	m_head = new BoardNode();
	m_head->m_board = new SenetBoard(dynamic_cast<const SenetBoard&>(curBoard));

	for (int i = 1; i < 5; i++){//no point of trying above 4 too much memory
		SearchResult res = m_search_algorithm->search(*m_head,0,i,SearchAlgorithm::DETER,my_timer,-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
		if (res.move.m_square == SEARCH_TIMEOUT){
//			printf("max depth = %d\n",i-1);
			break;
		}
		bestRes = res;
//		printf("searched = %d\n",i);
//		printf("nodes = %d size per node = %d => using %d bytes\n",
//				BoardHelper::my_num_nodes,
//				sizeof(SenetBoard::SquareData)* m_size_squares, //just an estimate
//				BoardHelper::my_num_nodes * sizeof(SenetBoard::SquareData)*m_size_squares);
//		fflush(stdout);
	}
	km = bestRes.move;

#ifdef __AI_GAME_SMART1_PLAYER_DEBUG__
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
	printf("With value: %lf\n",res.hval);

	printf("got %lf seconds left\n",my_timer.getRemainingMoveTime());
	fflush(stdout);
#endif

#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
	printf("Move: %s (%d)\n", Definitions::playerColorToString(m_myColor).c_str(), km.m_square);
#endif
}

void SenetSmart1Player::setName(const string &s)
{
	m_myName = s;
}

void SenetSmart1Player::initSearch(Utility *util,SearchAlgorithm *searcher){
	if (m_utility != NULL)
		delete m_utility;
	if (m_search_algorithm != NULL)
		delete m_search_algorithm;
	m_utility = util;
	m_search_algorithm = searcher;
	m_utility->initialize(m_size_squares,m_num_pieces);
	m_search_algorithm->initSearch(m_utility,m_utility,m_myColor,m_num_sticks);//later we'll use two different heuristics
}
