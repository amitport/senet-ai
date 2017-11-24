#include "SenetGame.h"
#include "SenetPlayer.h"

#include "UtilitySmart1.h"
#include "Expectimax.h"
#include "Star1.h"
#include "Star2.h"
#include "SenetSmart1Player.h"

SenetGame::SenetGame(vector<Player*> players, double player_initGame_time_limit,
					 GameTimer::TimeParams timeParams,
					 int num_squares, int num_pieces, 
					 int num_extra_safe, int num_extra_rebirth,
					 int num_sticks, unsigned int chance_seed,
					 int max_moves) : 
Game(players, player_initGame_time_limit, max_moves, timeParams), 
	m_size_squares(num_squares), m_num_pieces(num_pieces), m_num_extra_safe(num_extra_safe),
	m_num_extra_rebirth(num_extra_rebirth), m_num_sticks(num_sticks),
	m_WhiteNum(0), m_BlackNum(1)
{
	m_board = NULL;
	m_chance = new SenetChance(m_num_sticks, chance_seed);
}

SenetGame::~SenetGame()
{
	delete m_board;
	delete m_chance;
}

void SenetGame::initGame()
{
	// Timer for initialization only. Game timer parameters are
	// set in the constructor.
	GameTimer::TimeParams tp;
	tp.timePerGame_limit = false;
	tp.timePerMove_limit = true;
	tp.timePerMove = m_allowedInitGamePlayersTime;
	GameTimer gt(tp);

	m_board = new SenetBoard(m_size_squares, m_num_pieces, m_num_extra_safe,
		m_num_extra_rebirth, m_num_sticks, Definitions::WHITE);
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
	drawBoard();
#endif
	
	for(unsigned int i(0); i < m_Players.size(); ++i)
	{
#ifdef __AI_GAME_PROGRESS_INDICATOR__
		// Monitor player i initialization start
		FILE * tmpFile = fopen("curGameStat.txt", "a");
        fprintf(tmpFile, "P%di{", i+1);
        fclose(tmpFile);
#endif

		gt.startMoveTimer();
		(dynamic_cast<SenetPlayer*>(m_Players[i]))->
			initGame(m_size_squares, m_num_pieces, m_num_sticks, 
			*dynamic_cast<SenetBoard*>(m_board));
		if (i==0)
		{
			 (dynamic_cast<SenetSmart1Player*>(m_Players[0]))->initSearch(new UtilitySmart1(),new Expectimax());
			// (dynamic_cast<SenetSmart1Player*>(m_Players[0]))->initSearch(new UtilitySmart1(),new Star1());
			//(dynamic_cast<SenetSmart1Player*>(m_Players[0]))->initSearch(new UtilitySmart1(),new Star2());
		}
		else // (i==1)
		{
			//	(dynamic_cast<SenetSmart1Player*>(m_Players[1]))->initSearch(new UtilitySmart1(),new Star1());
		}
		if(gt.isMoveTimePassed(GameTimer::USE_OVERHEAD))
			m_gameRes.players_result[i] = GameRes::TIMEOUT;

#ifdef __AI_GAME_PROGRESS_INDICATOR__
		// Monitor player i initialization end
		tmpFile = fopen("curGameStat.txt", "a");
        fprintf(tmpFile, "}");
        fclose(tmpFile);
#endif
	};

}

bool SenetGame::isFinalState() const
{
	return (((dynamic_cast<SenetBoard*>(m_board))->getBoardResult()) != 
		SenetBoard::NOT_FINAL);
}

int SenetGame::getCurPlayer() const
{
	return getCurPlayerColor() == Definitions::WHITE ? 0 : 1;
}

Definitions::PlayerColor SenetGame::getCurPlayerColor() const
{
	return (dynamic_cast<SenetBoard*>(m_board))->getCurPlayerColor();
}

void SenetGame::makeMove(const Move &m)
{
	SenetBoard *kb(dynamic_cast<SenetBoard*>(m_board));
	kb->makeMove(kb->getCurPlayerColor(), (dynamic_cast<const SenetMove&>(m)));
}

void SenetGame::setNextChanceState()
{
	SenetBoard *kb(dynamic_cast<SenetBoard*>(m_board));
	ChanceState *p_scs = m_chance->getNextChanceState();
	SenetChanceState scs(p_scs);
	delete p_scs;
	kb->setChanceState(scs);
}

bool SenetGame::calcGameResult()
{
	// Move limit has passed for both players and calcGameResult already
	// been called
	if(m_gameRes.players_result[m_WhiteNum] == GameRes::DRAW &&
		m_gameRes.players_result[m_BlackNum] == GameRes::DRAW)
	{
		return true;
	};

	// White loses by technical offense, black wins
	if((m_gameRes.players_result[m_WhiteNum] == GameRes::ILLEGAL_MOVE) ||
		(m_gameRes.players_result[m_WhiteNum] == GameRes::TIMEOUT))
	{
		m_gameRes.players_result[m_BlackNum] = GameRes::WIN;
		return true;
	};

	// Black loses by technical offense, white wins
	if((m_gameRes.players_result[m_BlackNum] == GameRes::ILLEGAL_MOVE) ||
		(m_gameRes.players_result[m_BlackNum] == GameRes::TIMEOUT))
	{
		m_gameRes.players_result[m_WhiteNum] = GameRes::WIN;
		return true;
	};

	// Normal game state, get results from board state
	SenetBoard::BoardResult b((dynamic_cast<SenetBoard*>(m_board))->getBoardResult());
	switch(b)
	{
	case SenetBoard::NOT_FINAL:
		// Check if move limit has passed for all players
		// - in this case the game ends with a DRAW
		if(m_gameRes.players_result[m_WhiteNum] == GameRes::MOVE_LIMIT &&
			m_gameRes.players_result[m_BlackNum] == GameRes::MOVE_LIMIT)
		{
			m_gameRes.players_result[m_WhiteNum] = GameRes::DRAW;
			m_gameRes.players_result[m_BlackNum] = GameRes::DRAW;
		}
		else
		{
			if(m_gameRes.players_result[m_WhiteNum] != GameRes::MOVE_LIMIT)
				m_gameRes.players_result[m_WhiteNum] = GameRes::NONE;
			if(m_gameRes.players_result[m_BlackNum] != GameRes::MOVE_LIMIT)
				m_gameRes.players_result[m_BlackNum] = GameRes::NONE;
		};
		break;
	case SenetBoard::WIN_BLACK:
		m_gameRes.players_result[m_WhiteNum] = GameRes::NORMAL_LOSE;
		m_gameRes.players_result[m_BlackNum] = GameRes::NORMAL_WIN;
		break;
	case SenetBoard::WIN_WHITE:
		m_gameRes.players_result[m_WhiteNum] = GameRes::NORMAL_WIN;
		m_gameRes.players_result[m_BlackNum] = GameRes::NORMAL_LOSE;
		break;
	case SenetBoard::FINAL_DRAW:
		m_gameRes.players_result[m_WhiteNum] = GameRes::DRAW;
		m_gameRes.players_result[m_BlackNum] = GameRes::DRAW;
		break;
	default:
		throw;
	};
	return true;
}

SenetGame::GameRes::ResType SenetGame::getPlayerResult(int player_num) const
{
	return m_gameRes.players_result[player_num];
}

Move *SenetGame::getEmptyMove() const
{
	return new SenetMove;
}

Board *SenetGame::getCurBoardCopy() const
{
	Board *boardCopy = new SenetBoard(*dynamic_cast<const SenetBoard*>(m_board));
	return boardCopy;
}

void SenetGame::drawBoard() const
{
	vector<string> playersNames;
	for(unsigned int i(0); i < m_Players.size(); ++i)
		playersNames.push_back(m_Players[i]->getName());

	m_board->drawBoard(playersNames);
}

bool SenetGame::isChanceGame() const
{
	return true;
}

bool SenetGame::isTurnFinished() const
{
	return (dynamic_cast<const SenetBoard*>(m_board))->isTurnFinished();
}
