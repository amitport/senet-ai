#include "Definitions.h"
#include "SenetGame.h"

#include "SenetRandomPlayer.h"
#include "SenetStupidPlayer.h"
#include "SenetSmart1Player.h"
#include "SenetInteractivePlayer.h"

#include "GameTimer.h"

#include <iostream>
#include <cstdlib>
#include <cerrno>
#ifndef WIN32
#include <time.h>
#include <sys/time.h>
extern "C" void __libc_freeres(void);
#endif

using namespace std;

// Game parameters defaults
const int default_size_squares(30);					// The number of board squares
const int default_num_pieces(5);					// The number of pieces of each player
													//	at the beginning of the game
const int default_num_extra_safe(0);				// The number of extra safe squares	
const int default_num_extra_rebirth(0);				// The number of extra rebirth squares
const int default_num_sticks(6);					// The number of sticks
const int default_max_moves(100);					// The number of allowed moves per player
const unsigned int default_chance_seed(0);			// The random generator's seed
const double default_time_per_move(20);				// Time per player's turn (in seconds)
const double default_time_for_constructing(1);		// Time for player's constructor
const double default_time_for_initialization(10);	// Time for player's initGame

// Globals for replacing default values with command line parameters
int size_squares(default_size_squares);
int num_pieces(default_num_pieces);
int num_extra_safe(default_num_extra_safe);
int num_extra_rebirth(default_num_extra_rebirth);
int num_sticks(default_num_sticks);
int chance_seed(default_chance_seed);
int max_moves(default_max_moves);
double time_per_move(default_time_per_move);

// Parse command line
// -squares			Board squares
// -pieces			Initial number of pieces per player
// -extra_safe		Number of extra safe squares
// -extra_rebirth	Number of extra rebirth squares
// -sticks			Number of sticks
// -seed			Random generator seed
// -moves			Number of maximum moves per player
//					(multiple moves due to sticks outcome are not counted)
// -time			Time per game move
bool ProcessCommandLine(int argc, char **argv);

// Output command line parameters' description
void PrintHelpMessage(char **argv);

int main(int argc, char **argv)
{
#ifdef __AI_GAME_PROGRESS_INDICATOR_TOUR__
	// Monitor the game start time
#ifndef WIN32
	char buffer[30];
	struct timeval tv;
	time_t curtime;
	gettimeofday(&tv, NULL);
	curtime=tv.tv_sec;
	strftime(buffer,30,"%d-%m-%Y  %T",localtime(&curtime));

	FILE * tmpFile2 = fopen("../AllGameStat.txt", "a");
	fprintf(tmpFile2, "%s\n", buffer);
	fclose(tmpFile2);
#endif
#endif

	// Monitor game start
#ifdef __AI_GAME_PROGRESS_INDICATOR__
	FILE *tmpFile = fopen("curGameStat.txt", "w");
	fprintf(tmpFile, "Ps");
	fclose(tmpFile);
#endif

	// Read game parameters from command line, if any
	// Game uses defaults for any command line parameter not specified
	if(!ProcessCommandLine(argc, argv))
	{
		PrintHelpMessage(argv);
		return 1;
	};

	srand( (unsigned)time( NULL ) );

	// Game/move parameters
	GameTimer::TimeParams tp;
	tp.timePerGame_limit = false;
	tp.timePerMove_limit = true;
	tp.timePerMove = time_per_move;

	// Players construction
	// (initialization with game parameters is done as the first task
	// of SenetGame::playGame)
	bool Player1_bad_init(false), Player2_bad_init(false);
	// Time limits for construction, using timer's move limit
	GameTimer::TimeParams init_tp;
	init_tp.timePerGame_limit = false;
	init_tp.timePerMove_limit = true;
	init_tp.timePerMove = default_time_for_constructing;
	GameTimer init_timer(init_tp);

	// Constructing first player
	// It is white and will play first
#ifdef __AI_GAME_PROGRESS_INDICATOR__
	// Monitor start of player 1 construction
	tmpFile = fopen("curGameStat.txt", "a");
	fprintf(tmpFile, "P1c{");
	fclose(tmpFile);
#endif
	/*********************************
	 * FIRST PLAYER INITIALIZATION   *
	 *********************************/
	init_timer.startMoveTimer();
	Player *p1 = new SenetSmart1Player(Definitions::WHITE, tp, max_moves);
	(dynamic_cast<SenetSmart1Player*>(p1))->setName("Smart1 - White - Expectimax");
	if(init_timer.isMoveTimePassed(GameTimer::USE_OVERHEAD))
		Player1_bad_init = true;
#ifdef __AI_GAME_PROGRESS_INDICATOR__
	// Monitor end of player 1 construction
	tmpFile = fopen("curGameStat.txt", "a");
	fprintf(tmpFile, "}");
	fclose(tmpFile);
#endif

	// Constructing second player
	// It is black and will play second
#ifdef __AI_GAME_PROGRESS_INDICATOR__
	// Monitor start of player 2 construction
	tmpFile = fopen("curGameStat.txt", "a");
	fprintf(tmpFile, "P2c{");
	fclose(tmpFile);
#endif
	/**********************************
	 * SECOND PLAYER INITIALIZATION   *
	 **********************************/
	init_timer.startMoveTimer();
	Player *p2 = new SenetStupidPlayer(Definitions::BLACK, tp, max_moves);
	(dynamic_cast<SenetStupidPlayer*>(p2))->setName("Stupid Player - Black");
//	Player *p2 = new SenetInteractivePlayer(Definitions::BLACK, tp, max_moves);
//	(dynamic_cast<SenetInteractivePlayer*>(p2))->setName("Interactive Player - Black");
	if(init_timer.isMoveTimePassed(GameTimer::USE_OVERHEAD))
		Player2_bad_init = true;
#ifdef __AI_GAME_PROGRESS_INDICATOR__
	// Monitor end of player 2 construction
	tmpFile = fopen("curGameStat.txt", "a");
	fprintf(tmpFile, "}");
	fclose(tmpFile);
#endif
	
	// Check whether any player used too much time during construction
	if(Player1_bad_init || Player2_bad_init)
	{
		vector<string> playersNames;
		playersNames.push_back(p1->getName());
		playersNames.push_back(p2->getName());

		// Compute game results (premature termination - technical lose)
		Game::GameRes gameRes;
		gameRes.players_result.resize(2);

		if(Player1_bad_init)
			// Player 1 timeout
			gameRes.players_result[0] = Game::GameRes::TIMEOUT;
		else
			// Player 1 OK, player 2 timeout ==> Player 1 wins
			gameRes.players_result[0] = Game::GameRes::WIN;

		if(Player2_bad_init)
			// Player 2 timeout
			gameRes.players_result[1] = Game::GameRes::TIMEOUT;
		else
			// Player 2 OK, player 1 timeout ==> Player 2 wins
			gameRes.players_result[1] = Game::GameRes::WIN;

		delete p1;
		delete p2;

		// Output game result to the console
		cout << Game::ResultToString(gameRes, playersNames) << endl;
		// Exit program
#ifdef __AI_GAME_PROGRESS_INDICATOR__
		// Monitor exit program
		tmpFile = fopen("curGameStat.txt", "a");
        fprintf(tmpFile, "E");
        fclose(tmpFile);
#endif
		return 0;
	}

	// Players' construction went fine, start a game
	vector<Player*> players;
	players.push_back(p1);
	players.push_back(p2);

#ifdef __AI_GAME_PROGRESS_INDICATOR__
	// Monitor game start
	tmpFile = fopen("curGameStat.txt", "a");
	fprintf(tmpFile, "G{");
	fclose(tmpFile);
#endif
	// Initialize game
	SenetGame cng(players, default_time_for_initialization,
		tp, size_squares, num_pieces, num_extra_safe, num_extra_rebirth,
		num_sticks, chance_seed, max_moves);

	// Play game
	cng.playGame();
#ifdef __AI_GAME_PROGRESS_INDICATOR__
	// Monitor game end
	tmpFile = fopen("curGameStat.txt", "a");
	fprintf(tmpFile, "}");
	fclose(tmpFile);
#endif

	// Output game result to the console
	cout << cng.ResultToString() << endl;
#ifdef __AI_GAME_PROGRESS_INDICATOR__
	// Monitor exit program
	tmpFile = fopen("curGameStat.txt", "a");
	fprintf(tmpFile, "E");
	fclose(tmpFile);
#endif

	delete p2;
	delete p1;
#ifndef WIN32
	atexit(__libc_freeres);
#endif
	return 0;
}

bool ProcessCommandLine(int argc, char **argv)
{
	bool errorEncountered(false);

	for(int i(1); (i < argc) && !errorEncountered; ++i)
	{
		if(string(argv[i]) == "-h")
		{
			errorEncountered = true;
		}
		else if(string(argv[i]) == "-squares")
		{
			errno = 0;
			size_squares = atoi(argv[++i]);
			if(size_squares < 1)
				errorEncountered = true;
		}
		else if(string(argv[i]) == "-pieces")
		{
			errno = 0;
			num_pieces = atoi(argv[++i]);
			if((num_pieces < 1) || (num_pieces > size_squares/2))
				errorEncountered = true;
		}
		else if(string(argv[i]) == "-extra_safe")
		{
			errno = 0;
			num_extra_safe = atoi(argv[++i]);
			if(num_extra_safe < 0)
				errorEncountered = true;
		}
		else if(string(argv[i]) == "-extra_rebirth")
		{
			errno = 0;
			num_extra_rebirth = atoi(argv[++i]);
			if(num_extra_rebirth < 0)
				errorEncountered = true;
		}
		else if(string(argv[i]) == "-sticks")
		{
			errno = 0;
			num_sticks = atoi(argv[++i]);
			if(num_sticks < 1)
				errorEncountered = true;
		}
		else if(string(argv[i]) == "-seed")
		{
			errno = 0;
			chance_seed = (unsigned int)atoi(argv[++i]);
			// no error encountered - everything is a legitimate seed
		}
		else if (string(argv[i]) == "-moves")
		{
			errno = 0;
			max_moves = atoi(argv[++i]);
			if(max_moves < 0)
				errorEncountered = true;
		}
		else if(string(argv[i]) == "-time")
		{
			errno = 0;
			time_per_move = atof(argv[++i]);
			if(time_per_move <= 0)
				errorEncountered = true;
		}
		else
		{
			errorEncountered = true;
		};
		if(errno != 0)
			errorEncountered = true;
	};
	
	return !errorEncountered;
}

void PrintHelpMessage(char **argv)
{
	cout << "Arguments for " << argv[0] << " :" << endl;
	cout
		<< "\t -squares <board squares> (default: " << default_size_squares << ")" << endl
		<< "\t -pieces <number of pieces> (default: " << default_num_pieces << ")" << endl
		<< "\t -extra_safe <number of squares> (default: " << default_num_extra_safe << ")" << endl
		<< "\t -extra_rebirth <number of squares> (default: " << default_num_extra_rebirth << ")" << endl
		<< "\t -sticks <number of sticks> (default: " << default_num_sticks << ")" << endl
		<< "\t -seed <chance seed> (default: " << default_chance_seed << ")" << endl
		<< "\t -moves <maximal number of moves per player> (default: " << default_max_moves << ")" << endl
		<< "\t -time <time limit per move, in seconds> (default: " << default_time_per_move << ")" << endl
		;
}
