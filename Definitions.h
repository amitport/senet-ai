/* Definitions.h
 *
 * Global game definitions.
 */

#ifndef __DEFINITIONS_H__
#define __DEFINITIONS_H__

#include <string>

// The __AI_GAME_SYSTEM_CONSOLE_DEBUG__ flag allows some debugging.
// It prints the board state before and after each move,
// prints the current player and his move,
// and prints whether that move is legal, illegal or a timeout occurred,
// as well as other game specific information.
//#define __AI_GAME_SYSTEM_CONSOLE_DEBUG__

// The __AI_GAME_PROGRESS_INDICATOR__ enables game progress monitoring.
// It writes the current game status into ./curGameStat.txt .
//#define __AI_GAME_PROGRESS_INDICATOR__

// The __AI_GAME_PROGRESS_INDICATOR_TOUR__ enables game progress monitoring.
// It writes the current game status into ../AllGameStat.txt .
// This flag is only intended for use during the tournament.
//#define __AI_GAME_PROGRESS_INDICATOR_TOUR__

#ifdef __AI_GAME_PROGRESS_INDICATOR_TOUR__
#define __AI_GAME_PROGRESS_INDICATOR__
#endif

class Definitions
{
public:
	// Players' available colors.
	typedef enum{NONE, WHITE, BLACK} PlayerColor;

	// Get the opposite player of p.
	// p = WHITE returns BLACK
	// p = BLACK returns WHITE
	// p = NONE returns NONE
	static Definitions::PlayerColor getOppositePlayer(PlayerColor p);

	// Returns a character for player p's pieces
	static char playerColorToChar(PlayerColor p);

	// Returns a string for player p's color
	static std::string playerColorToString(PlayerColor p);
};

#endif
