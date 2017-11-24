#include "Definitions.h"

using std::string;

Definitions::PlayerColor Definitions::getOppositePlayer(Definitions::PlayerColor p)
{
	if(p == Definitions::NONE)
		return Definitions::NONE;

	return p == Definitions::WHITE ? Definitions::BLACK : Definitions::WHITE;
}

char Definitions::playerColorToChar(PlayerColor p)
{
	if(p == Definitions::NONE)
		return '-';
	if(p == Definitions::WHITE)
		return 'O';
	if(p == Definitions::BLACK)
		return 'X';
	return ' ';	// to disable warning
}

string Definitions::playerColorToString(PlayerColor p)
{
	if(p == Definitions::NONE)
		return "NONE";
	if(p == Definitions::WHITE)
		return "WHITE";
	if(p == Definitions::BLACK)
		return "BLACK";
	return "";	// to disable warning
}

