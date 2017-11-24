/* SenetBoard.h
 *
 * Concrete Board class for the Senet game.
 */

#ifndef __SENET_BOARD_H__
#define __SENET_BOARD_H__

#include "Board.h"
#include "SenetMove.h"
#include "SenetChance.h"

#include <vector>
#include <string>

using std::vector;

class SenetBoard : public Board
{
public:
	// Square types:
	//	NORMAL: No special function
	//	SAFE: Piece cannot be removed
	//	REBIRTH_FROM: Arriving piece removed and replaced in another square
	//	REBIRTH_TO: Pieces are replaced here
	typedef enum {NORMAL, SAFE, REBIRTH_FROM, REBIRTH_TO} SquareType;

	// Square data struct
	typedef struct
	{
		Definitions::PlayerColor player_color;	// player's piece
		SquareType square_type;					// square type
		int rebirth_to;							// where to pieces are reborn, if square_type==REBIRTH_FROM
												// meaningless otherwise
		int rebirth_id;							// if square_type==REBIRTH_*, the id of this FROM/TO pair
												// meaningless otherwise
		std::string toString() const;			// convert the square data into a printable string
	} SquareData;

	// Board state B[square]
	// B[0] is the first square, B[end] is the last square (one before
	// out of board)
	typedef vector<SquareData> BoardState;

	// Board result state
	// WIN_x implies opposite_player(x) has lost
	typedef enum{NOT_FINAL, WIN_BLACK, WIN_WHITE, FINAL_DRAW} BoardResult;

	// Default Constructor
	SenetBoard();
	// Initializes the board with <num_squares> squares,
	// <num_pieces> pieces for each player,
	// random <num_extra_safe> safe squares (in addition to the original),
	// random <num_extra_rebirth> rebirth squares (in addition to the original),
	// <num_sticks> number of sticks
	// <openning_player> first player to move
	SenetBoard(int num_squares, int num_pieces, 
		int num_extra_safe, int num_extra_rebirth,
		int num_sticks, Definitions::PlayerColor openning_player);
	// Copy Constructor
	// Copies board's parameters and state from kb
	SenetBoard(const SenetBoard &kb);

	// Virtual Destructor
	virtual ~SenetBoard();

	// Returns true iff move m of player p is legal in the current board state
	bool isLegalMove(const Definitions::PlayerColor p, const Move& m) const;
	// Updates the board with move m of player p.
	// Not guarranteed to check if m is a legal move.
	void makeMove(const Definitions::PlayerColor p, const Move& m);
	// Outputs the current board state to the console, using playersNames
	// to describe players (their order in playersNames should correspond
	// to the order in which they are saved in the game class)
	void drawBoard(const vector<string> &playersNames) const;
	// Draw board with each row in alternate direction.
	// If <alternateDirection>==true then first row is from left to right,
	// second from right to left, third from left to right and so on.
	// If <alternateDirection>==false then all rows are from left to right.
	void drawBoard(const vector<string> &playersNames, 
		bool alternateDirection) const;

	// Returns true iff the empty move is legal in the current state
	// An empty move is legal if sticks state is 0, or if no move
	// is legal with sticks state > 0
	bool isLegalEmptyMove(const Definitions::PlayerColor p) const;

	// Returns true iff the turn has ended - i.e. the sticks' value
	// does not grant another move in this turn
	bool isTurnFinished() const;

	// Computes the current board result:
	// NOT_FINAL: Both players still have pieces on the board
	// WIN_WHITE: WHITE has removed all of its pieces off the board
	// WIN_BLACK: BLACK has removed all of its pieces off the board
	// FINAL_DRAW: DRAW (cannot happen)
	BoardResult getBoardResult() const;

	// Returns the current board square state at B[square]
	// No boundary checks
	SquareData getBoardState(int square) const;

	// Returns the SenetMoves which are valid successors for the 
	// current board state for player <p>
	vector<SenetMove> getSuccessors(Definitions::PlayerColor p) const;

	// Returns the length of the sequence of player p's pieces,
	// starting at B[start_square], checking towards the board's end
	int checkSequence(const Definitions::PlayerColor &p,
		int start_square) const;

	// Returns the current sticks' state
	// A number between 0 and m_num_sticks
	int getSticksState() const;

	// Updates the current chance state
	void setChanceState(const ChanceState &chance_state);

	// Returns the closest empty square to m_boardState[square].rebirth_to
	// (index lower or equal to) which is empty
	// If no such square exists, returns <square>
	int findAvailableRebirthToSquare(int square) const;

	// Returns the current player
	Definitions::PlayerColor getCurPlayerColor() const;

private:
	// Sets B[from] to be a rebirth square pointing to B[to]
	void setRebirth(int from, int to);

	// Sets next player according to the sticks state
	void setNextPlayer();

	// Get non empty move successors
	vector<SenetMove> getNonEmptySuccessors(Definitions::PlayerColor p) const;

	// Get the number of pieces with color p on board
	int countPieces(Definitions::PlayerColor p) const;

	// Enable silent isLegalMove even if __AI_GAME_SYSTEM_CONSOLE_DEBUG__ is defined
	bool isLegalMove(const Definitions::PlayerColor p, const Move& m, bool silent) const;

	// Add random safe and rebirth squares
	void addRandomSafeRebirth(int num_safe, int num_rebirth);

	int m_size_squares;			// The number of board squares
	int m_num_pieces;			// The number of pieces of each player
								//	at the beginning of the game
	int m_num_extra_safe;		// The number of extra safe squares
	int m_num_extra_rebirth;	// The number of extra rebirth squares
	int m_num_sticks;			// The number of sticks
	BoardState m_boardState;	// The board's state

	static const int safe_seq = 2;		// Minimal length of a sequence from
										// which a piece cannot be removed
	static const int block_seq = 3;		// Minimal length of a sequence that
										// blocks jumping over it
	int m_cur_rebirth_id;		// Counts rebirth squares, for printing

	Definitions::PlayerColor m_curPlayer;	// Current player
};

#endif
