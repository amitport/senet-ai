#include "Game.h"
#include <cstdlib>

Game::Game(vector<Player*> players, double player_initGame_time_limit,
		   int player_moves_limit, GameTimer::TimeParams timeParams) :
m_board(0), m_chance(0), m_Players(players), m_gameTimer(timeParams),
m_allowedInitGamePlayersTime(player_initGame_time_limit),
m_playerMoveNumberLimit(player_moves_limit)
{
	for(unsigned int i(0); i < players.size(); ++i)
		m_PlayersRemainingMoves.push_back(m_playerMoveNumberLimit);
}

Game::~Game()
{
}

void Game::playGame()
{
	m_gameRes.players_result.resize(m_Players.size());
	for(unsigned int i(0); i < m_Players.size(); ++i)
		m_gameRes.players_result[i] = GameRes::NONE;

	this->initGame();
	calcGameResult();	// check if no player timeoutted on initialization

	int numPlayersStanding(m_Players.size());
	// check for players with bad initializations
	for(unsigned int i(0); i < m_Players.size(); ++i)
		if(m_gameRes.players_result[i] != GameRes::NONE)
			--numPlayersStanding;

#ifdef __AI_GAME_PROGRESS_INDICATOR__
	// Monitor game start
	FILE * tmpFile = fopen("curGameStat.txt", "a");
	fclose(tmpFile);
#endif

	while(!isFinalState() && (numPlayersStanding>0))
	{
		int curPlayer = this->getCurPlayer();

		if(m_gameRes.players_result[curPlayer] == GameRes::MOVE_LIMIT)
		{
			Move * emptyMove(this->getEmptyMove());
			this->makeMove(*emptyMove);
			delete emptyMove;
			continue;
		};

		if(m_gameRes.players_result[curPlayer] == GameRes::NONE)
		{
#ifdef __AI_GAME_PROGRESS_INDICATOR__
			// Monitor current player index
			FILE * tmpFile = fopen("curGameStat.txt", "a");
			fprintf(tmpFile, "%d", curPlayer+1);
			fclose(tmpFile);
#endif
			this->setNextChanceState();
			Board * boardCopy = this->getCurBoardCopy();
			Move * playerMove(this->getEmptyMove());
			m_gameTimer.startMoveTimer();
			m_Players[curPlayer]->makeMove(*boardCopy, *playerMove);
			if(m_gameTimer.isMoveTimePassed(GameTimer::USE_OVERHEAD))
			{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
				printf("TIMEOUT\n");
#endif
				m_gameRes.players_result[curPlayer] = GameRes::TIMEOUT;
			}
			else if(m_board->isLegalMove(this->getCurPlayerColor(), *playerMove))
			{
				// this-> knows which player should play next
				this->makeMove(*playerMove);
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
				printf("LEGAL\n");
				this->drawBoard();
#endif
			}
			else
			{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
				printf("ILLEGAL\n");
#endif
				m_gameRes.players_result[curPlayer] = GameRes::ILLEGAL_MOVE;
			};
			delete playerMove;
			delete boardCopy;

			if(!this->isTurnFinished())
				continue;

			--m_PlayersRemainingMoves[curPlayer];
//			if (m_PlayersRemainingMoves[curPlayer]%6==0)
//			{
//#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
//				printf("Ramaining moves: %d\n",m_PlayersRemainingMoves[curPlayer]);////////////////////////////////////////////////////////////////////////////
//				this->drawBoard();
//#endif
//			}
			if(m_gameRes.players_result[curPlayer] == GameRes::NONE)
				if(m_PlayersRemainingMoves[curPlayer] == 0)
					m_gameRes.players_result[curPlayer] = GameRes::MOVE_LIMIT;
		};

		calcGameResult();

		int lastPlayerStanding(-1);
		numPlayersStanding = 0;
		for(unsigned int i(0); i < m_Players.size(); ++i)
		{
			if(m_gameRes.players_result[i] == GameRes::NONE)
			{
				lastPlayerStanding = i;
				++numPlayersStanding;
			}
		};
	};

#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
	printf("FINAL\n");
	this->drawBoard();
#endif

	calcGameResult();

#ifdef AMIT_STATS
	// Monitor the game results
	FILE * tmpFile2 = fopen("AmitStat.txt", "a");
	int res;
	switch (m_gameRes.players_result[0]){
		case GameRes::DRAW:
			res = 0;
			break;
		case GameRes::NORMAL_WIN:
			res = 1;
			break;
		case GameRes::NORMAL_LOSE:
			res = 2;
			break;
		case GameRes::WIN:
			res = 8;
			break;
		default:
			res = 9;
	}
	fprintf(tmpFile2, "%d\r\n", res);
	fclose(tmpFile2);
#endif
}

string Game::ResultToString() const
{
	vector<string> playersNames;
	for(unsigned int i(0); i < m_Players.size(); ++i)
		playersNames.push_back(m_Players[i]->getName());
	return ResultToString(m_gameRes, playersNames);
}

string Game::ResultToString(const GameRes &gameRes,
							const vector<string> &playersNames)
{
	string resStr = "";
	for(unsigned int i(0); i < gameRes.players_result.size(); ++i)
	{
		// Some players will not report their correct name,
		// and we actually ignore this value anyway
		resStr += playersNames[i] + " ";
		switch(gameRes.players_result[i])
		{
		case Game::GameRes::NORMAL_WIN:
			resStr += string("NORMAL_WIN") + string("\t");
			break;
		case Game::GameRes::NORMAL_LOSE:
			resStr += string("NORMAL_LOSE") + string("\t");
			break;
		case Game::GameRes::WIN:
			resStr += string("WIN") + string("\t");
			break;
		case Game::GameRes::TIMEOUT:
			resStr += string("TIMEOUT") + string("\t");
			break;
		case Game::GameRes::ILLEGAL_MOVE:
			resStr += string("ILLEGAL_MOVE") + string("\t");
			break;
		case Game::GameRes::DRAW:
			resStr += string("DRAW") + string("\t");
			break;
        case Game::GameRes::NONE:
            resStr += string("NONE") + string("\t");
            break;
		case Game::GameRes::MOVE_LIMIT:
			resStr += string("MOVE_LIMIT") + string("\t");
			break;
        default:
			throw;
            break;
		};
	};
	return resStr;
}
