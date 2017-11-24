
#include "SmartUtility.h"
#include "Star1.h"
#include "SmartSenetPlayer.h"

using namespace senet_ai;

SmartSenetPlayer::SmartSenetPlayer(Definitions::PlayerColor playerColor,
									 const GameTimer::TimeParams &tp, int moves_limit)
: SenetPlayer(playerColor, tp, moves_limit) , prev_total_size(0)
{
	m_utility = NULL;
	m_light_utility = NULL;
	m_search_algorithm = NULL;
	m_head = NULL;

#ifdef AMIT_STATS //used for experiments
	movesMade = 0;
	totalDepth = 0;
	totalTime = 0.0;

	for (int i = 0; i < 10; i++)
	{
		movesPerDepth[i] = 0;
		timePerDepth[i] = 0.0;
	}
#endif
}

SmartSenetPlayer::~SmartSenetPlayer()
{
	if (m_utility != NULL)
		delete m_utility;
	if (m_light_utility != NULL)
		delete m_light_utility;
	if (m_search_algorithm != NULL)
		delete m_search_algorithm;
	if (m_head != NULL)
		delete m_head;
}

void SmartSenetPlayer::initGame(int num_squares, int num_pieces,
					   int num_sticks, const SenetBoard &initial_board)
{
	setName("SmartSenetPlayer");
	m_size_squares = num_squares;
	m_num_pieces = num_pieces;
	m_num_sticks = num_sticks;
	BoardHelper::init(m_size_squares,
			sizeof(BoardNode)+
			sizeof(SenetBoard)+
			sizeof(SenetChanceState)+
			/* multiply size_squares by 2 to be on the safe side (we don't really know the vector's size) */
			sizeof(SenetBoard::SquareData)* m_size_squares * 2 );

	//start the search parameter with default values
	initSearch(new SmartUtility(), new SmartUtility(), new Star1(),5);
}

void SmartSenetPlayer::makeMove(const Board& curBoard, Move &myMove)
{
	GameTimer my_timer(m_gameTimerParams);
	my_timer.startMoveTimer();

	SenetMove &km(dynamic_cast<SenetMove&>(myMove));
	BoardHelper::my_total_size = prev_total_size;
	SearchResult bestRes;
	if (NULL != m_head){
		//delete the cache from previous move
		delete m_head;
	}

	m_head = new BoardNode();
	m_head->m_board = new SenetBoard(dynamic_cast<const SenetBoard&>(curBoard));
	m_head->updateBoardFields();

	int i;
	//start iterative deepening
	for (i = 1; i <= m_searchDepth; i++)
	{
		//run the search to a limited depth
		SearchResult res = m_search_algorithm->search(*m_head,0,i,SearchAlgorithm::DETER,my_timer,-std::numeric_limits<double>::infinity(),std::numeric_limits<double>::infinity());
		if (res.move.m_square == SearchAlgorithm::SEARCH_TIMEOUT){
			//check if we got a timeout and exit
			break;
		}
#ifdef AMIT_STATS
		//update some statistics
		movesPerDepth[i]++;
		timePerDepth[i] += m_gameTimerParams.timePerMove - my_timer.getRemainingMoveTime();
#endif
		//save our best result so far
		bestRes = res;
	}

	//this is for our experiments
	//BoardHelper::my_total_size is static in Boardhelper we
	//need to save it in the player and retrieve it later
	//(note that we are not removing any code lines at this point
	//because we want to keep it the same as in our tests)
	prev_total_size = BoardHelper::my_total_size;
	BoardHelper::my_total_size = 0;
	km = bestRes.move;

	//just for statistics
#ifdef AMIT_STATS
	movesMade++;
	totalDepth += i-1;
	totalTime += m_gameTimerParams.timePerMove - my_timer.getRemainingMoveTime();
#endif
}

void SmartSenetPlayer::setName(const std::string &s)
{
	m_myName = s;
}

//initiate the search parameters (see Search algorithm)
void SmartSenetPlayer::initSearch(Utility *util, Utility *light_util, SearchAlgorithm *searcher,int searchDepth){
	if (m_utility != NULL)
		delete m_utility;
	if (m_light_utility != NULL)
		delete m_light_utility;
	if (m_search_algorithm != NULL)
		delete m_search_algorithm;

	m_utility = util;
	m_light_utility = light_util;
	m_search_algorithm = searcher;
	m_utility->initialize(m_size_squares,m_num_pieces,m_num_sticks);
	m_light_utility->initialize(m_size_squares,m_num_pieces,m_num_sticks);
	m_search_algorithm->initSearch(m_utility,m_light_utility,m_myColor,m_num_sticks);
	m_searchDepth = searchDepth;
}
