#include "SenetBoard.h"
#include "SenetMove.h"
#include <cstdio>
#include <cmath>
#include <algorithm>
#include <sstream>

SenetBoard::SenetBoard()
{
	*this = SenetBoard(0, 0, 0, 0, 0, Definitions::NONE);
}

SenetBoard::SenetBoard(const SenetBoard &kb)
{
	if(&kb != this)
	{
		m_size_squares = kb.m_size_squares;
		m_num_pieces = kb.m_num_pieces;
		m_num_extra_safe = kb.m_num_extra_safe;
		m_num_extra_rebirth = kb.m_num_extra_rebirth;
		m_num_sticks = kb.m_num_sticks;
		m_curPlayer = kb.m_curPlayer;
		m_boardState = kb.m_boardState;
		m_chance_state = new SenetChanceState(kb.m_chance_state);
	};
}

SenetBoard::~SenetBoard()
{
	delete m_chance_state;
}

SenetBoard::SenetBoard(int num_squares, int num_pieces, 
					   int num_extra_safe, int num_extra_rebirth,
					   int num_sticks, Definitions::PlayerColor openning_player)
: m_size_squares(num_squares), m_num_pieces(num_pieces),
   m_num_extra_safe(num_extra_safe), m_num_extra_rebirth(num_extra_rebirth),
   m_num_sticks(num_sticks), m_cur_rebirth_id(0), m_curPlayer(openning_player)
{
	m_boardState.resize(num_squares);
	m_chance_state = new SenetChanceState();
	for(int i(0); i < num_squares; ++i)
	{
		m_boardState[i].square_type = NORMAL;
		m_boardState[i].player_color = Definitions::NONE;
	};
	for(int i(0); i < m_num_pieces * 2; ++i)
	{
		if(i % 2 == 0)
			m_boardState[i].player_color = Definitions::WHITE;
		else
			m_boardState[i].player_color = Definitions::BLACK;
	};
	m_boardState[m_size_squares - 2].square_type = SAFE;
	m_boardState[m_size_squares - 3].square_type = SAFE;
	setRebirth(m_size_squares - 4, std::max(m_size_squares - 4 - 12, 0));
	m_boardState[m_size_squares - 5].square_type = SAFE;
	addRandomSafeRebirth(m_num_extra_safe, m_num_extra_rebirth);
}

void SenetBoard::addRandomSafeRebirth(int num_safe, int num_rebirth)
{
	vector<int> v;
	Chance rng(num_safe + 7 * num_rebirth);
	for(int i(m_num_pieces*3); i < m_size_squares - 8; ++i)
	{
		if(m_boardState[i].square_type != NORMAL)
			continue;
		v.push_back(i);
	};
	
	for(int j = 0; j < 10; ++j)
	{
		for(unsigned int i(0); i < v.size() ; ++i)
		{
			int s = i;
			int d = rng.NextInt(i, v.size()-1);
			std::swap(v[s], v[d]);
		};
	};
	unsigned int curV(0);
	for(curV = 0; (curV < v.size()) && (num_safe > 0); ++curV, --num_safe)
		m_boardState[v[curV]].square_type = SAFE;
	for(; (curV < v.size()) && (num_rebirth > 0); ++curV, --num_rebirth)
	{
		int dst = std::max(v[curV]-12, 0);
		if(m_boardState[dst].square_type != NORMAL)
			++num_rebirth;
		else
			setRebirth(v[curV], dst);
	};
}

void SenetBoard::setRebirth(int from, int to)
{
	m_boardState[from].square_type = REBIRTH_FROM;
	m_boardState[from].rebirth_to = to;
	m_boardState[from].rebirth_id = ++m_cur_rebirth_id;
	m_boardState[to].square_type = REBIRTH_TO;
	m_boardState[to].rebirth_id = m_cur_rebirth_id;
}

bool SenetBoard::isLegalEmptyMove(const Definitions::PlayerColor p) const
{
	if(getSticksState() == 0)
		return true;
	
	// check if no successors are legal
	vector<SenetMove> succ(getNonEmptySuccessors(p));
	return succ.size() == 0;
}

bool SenetBoard::isLegalMove(const Definitions::PlayerColor p, const Move& m) const
{
	return isLegalMove(p, m, false);
}

bool SenetBoard::isLegalMove(const Definitions::PlayerColor p, const Move& m, bool silent) const
{
	const SenetMove &km(dynamic_cast<const SenetMove&>(m));
	if(km.isEmpty())
	{
		// An empty move is obligatory for sticks state 0
		if(isLegalEmptyMove(p))
			return true;
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
		if(!silent) printf("ILLEGAL MOVE: EMPTY MOVE\n");
#endif
		return false;
	};

	int move_to_square = km.m_square + getSticksState();

	// is source square in board
	if((km.m_square < 0) || (km.m_square >= m_size_squares))
	{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
		if(!silent) printf("ILLEGAL MOVE: OUT OF BOARD\n");
#endif
		return false;
	};
	// source square must be current player's piece
	if(m_boardState[km.m_square].player_color != p)
	{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
		if(!silent) printf("ILLEGAL MOVE: SOURCE IS OPPOSITE PLAYER OR IS EMPTY\n");
#endif
		return false;
	};
	// cannot jump over blocking sequence
	for(int i(km.m_square + 1); i < move_to_square - block_seq + 1; ++i)
	{
		if(checkSequence(Definitions::getOppositePlayer(p), i) >= block_seq)
		{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
			if(!silent) printf("ILLEGAL MOVE: TRYING TO JUMP OVER A BLOCKING SEQUENCE\n");
#endif
			return false;
		};
	};

	// remove piece from board, unless the player has
	// any pieces in the first third of the board
	if(move_to_square >= m_size_squares)
	{
		for(int i(0); i < m_size_squares / 3; ++i)
			if(m_boardState[i].player_color == p)
			{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
		if(!silent) printf("ILLEGAL MOVE: PLAYER HAS PIECES IN FIRST THIRD OF THE BOARD\n");
#endif
				return false;
			}
		return true;
	}

	// destination square must be opponent's piece or empty
	if(m_boardState[move_to_square].player_color == p)
	{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
		if(!silent) printf("ILLEGAL MOVE: DESTINATION IS CURRENT PLAYER\n");
#endif
		return false;
	};
	// cannot remove opponent's piece from a safe square
	if((m_boardState[move_to_square].player_color == Definitions::getOppositePlayer(p)) &&
		m_boardState[move_to_square].square_type == SAFE)
	{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
		if(!silent) printf("ILLEGAL MOVE: DESTINATION IS A SAFE CELL\n");
#endif
		return false;
	};
	// cannot remove opponent's piece which belongs to a safe sequence
	for(int i(move_to_square - 1); i <= move_to_square; ++i)
	{
		if(checkSequence(Definitions::getOppositePlayer(p), i) >= safe_seq)
		{
#ifdef __AI_GAME_SYSTEM_CONSOLE_DEBUG__
			if(!silent) printf("ILLEGAL MOVE: DESTINATION IS A PART OF A SAFE SEQUENCE\n");
#endif
			return false;
		};
	};

	return true;
}

void SenetBoard::makeMove(const Definitions::PlayerColor p, const Move& m)
{
	const SenetMove &km(dynamic_cast<const SenetMove&>(m));

	if(!m.isEmpty())
	{
		int move_to_square = km.m_square + getSticksState();
	
		if(move_to_square >= m_size_squares)
			m_boardState[km.m_square].player_color = Definitions::NONE;
		else
		{
			std::swap(m_boardState[km.m_square].player_color, m_boardState[move_to_square].player_color);
			if(m_boardState[move_to_square].square_type == REBIRTH_FROM)
				std::swap(m_boardState[move_to_square].player_color,
				m_boardState[findAvailableRebirthToSquare(move_to_square)].player_color);
		};
	};

	setNextPlayer();
}

void SenetBoard::setNextPlayer()
{
	if(!isTurnFinished())
	{
		m_curPlayer = m_curPlayer;
	}
	else
		m_curPlayer = Definitions::getOppositePlayer(m_curPlayer);
}

int SenetBoard::findAvailableRebirthToSquare(int square) const
{
	for(int i(m_boardState[square].rebirth_to); i >= 0; --i)
		if(m_boardState[i].player_color == Definitions::NONE)
			return i;
	return square;
}

int SenetBoard::checkSequence(const Definitions::PlayerColor &p,
								  int start_square) const
{
	if(	start_square < 0 
		|| start_square >= m_size_squares)
		return 0;

	if(m_boardState[start_square].player_color != p)
		return 0;

	int seqLen(0);
	for(int i(start_square); i < m_size_squares; ++i)
	{
		if(m_boardState[i].player_color != p)
			break;
		++seqLen;
	};
	return seqLen;
}

int SenetBoard::countPieces(Definitions::PlayerColor p) const
{
	int res(0);
	for(int i(0); i < m_size_squares; ++i)
		if(m_boardState[i].player_color == p) ++res;
	return res;
}

SenetBoard::BoardResult SenetBoard::getBoardResult() const
{
	BoardResult res;

	int white_pieces(countPieces(Definitions::WHITE));
	int black_pieces(countPieces(Definitions::BLACK));

	if(white_pieces == 0)
		res = WIN_WHITE;
	else if(black_pieces == 0)
		res = WIN_BLACK;
	else
		res = NOT_FINAL;

	return res;
}

string SenetBoard::SquareData::toString() const
{
	string res;
	res += Definitions::playerColorToChar(player_color);
	
	std::stringstream rebirth_id_str;
	rebirth_id_str << rebirth_id;

	if(square_type == SenetBoard::NORMAL)
		res += "--";
	else if(square_type == SenetBoard::SAFE)
		res += "S-";
	else if(square_type == SenetBoard::REBIRTH_FROM)
		res += "R" + rebirth_id_str.str();
	else if(square_type == SenetBoard::REBIRTH_TO)
		res += "r" + rebirth_id_str.str();

	return res;
}

void SenetBoard::drawBoard(const vector<string> &playersNames) const
{
	drawBoard(playersNames, false);
}

void SenetBoard::drawBoard(const vector<string> &playersNames,
						   bool alternateDirection) const
{
	printf("%8s : %c\n", playersNames[0].c_str(), Definitions::playerColorToChar(Definitions::WHITE));
	printf("%8s : %c\n", playersNames[1].c_str(), Definitions::playerColorToChar(Definitions::BLACK));

	int squares_per_row(10);

	for(int i(0); i < 8+4*squares_per_row/2-3; ++i)
		printf(" ");
	printf("%5s\n", "units");
	printf("%8s", "");
	for(int i(0); i < squares_per_row; ++i)
		printf("%4d", i);
	printf("\n");

	int num_rows(m_size_squares / squares_per_row);
	if(m_size_squares % squares_per_row != 0)
		++num_rows;

	for(int r(0); r < num_rows; ++r)
	{
		if(r == num_rows/2)
			printf("%-5s", "tens");
		else
			printf("%5s", "");
		printf("%3d", r);
		int first_col = 0,
			last_col = squares_per_row;
		int dir = 1;
		if((r % 2 == 1) && alternateDirection)
		{
			std::swap(first_col, last_col);
			--first_col;
			--last_col;
			dir = -1;
		}
		for(int c(first_col); c != last_col; c += dir)
		{
			int board_coord = r * squares_per_row + c;
			if(board_coord >= m_size_squares)
				printf("%4s", "");
			else
				printf("%4s", m_boardState[board_coord].toString().c_str());
		};
		printf("\n");
	};
	printf("\n");
}


vector<SenetMove> SenetBoard::getSuccessors(Definitions::PlayerColor p) const
{
	vector<SenetMove> v;
	SenetMove emptyMove;
	if(isLegalMove(p, emptyMove, true))
		v.push_back(emptyMove);

	vector<SenetMove> non_empty(getNonEmptySuccessors(p));
	for(unsigned int i(0); i < non_empty.size(); ++i)
		v.push_back(non_empty[i]);

	return v;
}

vector<SenetMove> SenetBoard::getNonEmptySuccessors(Definitions::PlayerColor p) const
{
	vector<SenetMove> v;

	for(int c(0); c < m_size_squares; ++c)
	{
		SenetMove move(c);
		// isLegalMove for Senet does not depend on color
		if(isLegalMove(p, move, true))
			v.push_back(move);
	};

	return v;
}

SenetBoard::SquareData SenetBoard::getBoardState(int square) const
{
	return m_boardState[square];
}

Definitions::PlayerColor SenetBoard::getCurPlayerColor() const
{
	return m_curPlayer;
}

int SenetBoard::getSticksState() const
{
	return dynamic_cast<SenetChanceState*>(m_chance_state)->num_ON_sticks;
}

void SenetBoard::setChanceState(const ChanceState &chance_state)
{
	delete m_chance_state;
	m_chance_state = new SenetChanceState(&chance_state);
}

bool SenetBoard::isTurnFinished() const
{
	bool res = true;
	
	int sticksState = getSticksState();
	res = res && (
		(sticksState == 1) ||
		(sticksState == (m_num_sticks * 2) / 3) ||
		(sticksState == m_num_sticks)
		);
	res = res && (countPieces(getCurPlayerColor()) > 0);
	return !res;
}

