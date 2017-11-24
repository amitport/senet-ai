/*
 * SmartUtility.cpp
 *
 *  Created on: Dec 24, 2009
 *      Author: shaharch
 */

#include "SmartUtility.h"
#include "Combi.h"

using namespace senet_ai;

SmartUtility::SmartUtility() {}

/** Reads the current board, get a current move,
 * Calculate utility function for suggested board */
double SmartUtility::calculate(const SenetBoard& curBoard, Definitions::PlayerColor playerColor)
{
	// Calculate the utility for attacking player. If (the_color != attacker) return its opposite value.

	// Consider:
	// 3Block is good - especially if opponent has close piece
	// 2Block is good
	// Total distance from end of board
	// Safe squares
	// Amount of possible moves for opponent (Attacker)
	// Probability to be stuck (Attacker)
	// Number of exposed pieces - or probabilty to be eaten (more for defender)

	// In advanced stages, try to advance all pieces from first trimester

	Definitions::PlayerColor attacker, defender;
	attacker = curBoard.getCurPlayerColor();
	defender = Definitions::getOppositePlayer(attacker);

    int i, j, k, strick_len;
    int at_pieces = 0, df_pieces = 0;		// No. of pieces
    bool at_third = false, df_third = false;// boolean, is player in first third
    double at_dist = 0, df_dist = 0;			// Distance from start
    int at_safe = 0, df_safe = 0;			// No. of safes
    int at_exposed = 0, df_exposed = 0;		// Dangerous tosses
    int at_blocked = (1<<m_num_sticks)-1, df_blocked = (1<<m_num_sticks)-1;	// Blocked tosses
    double at_moves = 0, df_moves = 0;		// No. of possible moves
    int at_2block = 0, df_2block = 0;		// No. of 2 blocks
    double at_3block = 0, df_3block = 0;		// No. of 3 blocks
    int at_bonus = 0, df_bonus = 0;			// Bonus for winning

    SenetBoard::SquareData square_data, next_square_data;
    Definitions::PlayerColor last_square = Definitions::NONE;

    for (i=0; i<m_size_squares; i++)
    {
    	square_data = curBoard.getBoardState(i);
    	if (square_data.player_color == attacker)
    	{
    		at_pieces++;
    		at_dist += (1+(m_size_squares-i)/(m_size_squares*4.))*(m_size_squares-i); //steps in the beginning are 1.25 more meaningful than later steps
    		if (i<(m_size_squares/3)) at_third=true;
    		if (square_data.square_type == SenetBoard::SAFE) at_safe++;
    		// Defense calculations -
    		if (df_pieces>0 && last_square!=attacker) // New sequence
    		{
				for (j=i+1; j<m_size_squares; j++)
					if (curBoard.getBoardState(j).player_color != attacker) break;
				strick_len = j-i;
				if (strick_len == 1) {}// Exposed
				else if (strick_len == 2) // 2Block
				{
					at_2block += 1;
				}
				else // 3Block
				{
					at_3block += (1 + (strick_len-3)/2.);
				}
    		}

    		// Offense calculations - look forward
    		for (j=1;j<=m_num_sticks;j++)
    		{
    			// End of board
    			if ((j+i)>=m_size_squares)
    			{
    				if (!at_third)
    					for (k=j; k<=m_num_sticks;k++)
    					{
    						at_blocked &= ~(1<<(k-1));	// free j, j+1, j+2 ... toss
    						at_moves += probs[k];
    					}
    				break;
    			}
    			// Inside Board
    			next_square_data = curBoard.getBoardState(i+j);
    			if (next_square_data.player_color == defender)
    			{
    				int block_size = 1;
    				j++;
    				while (((i+j)<m_size_squares) && (curBoard.getBoardState(i+j).player_color == defender))
    				{
    					block_size++;
						j++;
    				}
    				j--;	// Point to last piece in straight

    				if (block_size == 1)	// Exposed piece
    				{
    					if (curBoard.getBoardState(i+j).square_type != SenetBoard::SAFE)
    					{
    						at_blocked &= ~(1<<(j-1));	// free j toss
    						at_moves += probs[j];
    						df_exposed |= 1<<(j-1);		// defender is exposed for j toss
    					}
    					continue;
    				}
    				else if (block_size == 2)	// 2Block - check further tosses
    					continue;
    				else						// 3Block - no tosses to unblock
    					break;
    			}
    			else if (next_square_data.player_color == Definitions::NONE)
    			{
    				// maybe add rebirth
    				at_blocked &= ~((1<<j)-1);	// Free j toss
    				at_moves += probs[j];
    			}
    		}

    	}
    	else if (square_data.player_color == defender)
    	{
    		df_pieces++;
    		df_dist += (1+(m_size_squares-i)/(m_size_squares*4.))*(m_size_squares-i); //steps in the beginning are 1.25 more meaningful than later steps
    		if (i<(m_size_squares/3)) df_third=true;
    		if (square_data.square_type == SenetBoard::SAFE) df_safe++;
    		// Defense calculations
    		if (at_pieces>0 && last_square!=defender) // New sequence
    		{
				for (j=i+1; j<m_size_squares; j++)
					if (curBoard.getBoardState(j).player_color != defender) break;
				strick_len = j-i;
				if (strick_len == 1) {}// Exposed
				else if (strick_len == 2) // 2Block
				{
					df_2block += 1;
				}
				else // 3Block
				{
					df_3block += (1 + (strick_len-3)/2.);
				}
    		}
    		// Offense calculations - look forward
    		for (j=1;j<=m_num_sticks;j++)
    		{
    			// End of board
    			if ((j+i)>=m_size_squares)
    			{
    				if (!df_third)
    					for (k=j; k<=m_num_sticks;k++)
    					{
    						df_blocked &= ~(1<<(k-1));	// free j, j+1, j+2 ... toss
							df_moves += probs[k];
    					}
    				break;
    			}
    			// Inside Board
    			next_square_data = curBoard.getBoardState(i+j);
    			if (next_square_data.player_color == attacker)
    			{
    				int block_size = 1;
    				j++;
    				while (((i+j)<m_size_squares) && (curBoard.getBoardState(i+j).player_color == attacker))
    				{
    					block_size++;
						j++;
    				}
    				j--;	// Point to last piece in sequence

    				if (block_size == 1)	// Exposed piece
    				{
    					if (curBoard.getBoardState(i+j).square_type != SenetBoard::SAFE)
    					{
    						df_blocked &= ~(1<<(j-1));	// free j toss
    						df_moves += probs[j];
    						at_exposed |= 1<<(j-1);		// attacker is exposed for j toss
    					}
    					continue;
    				}
    				else if (block_size == 2)	// 2Block - check further tosses
    					continue;
    				else						// 3Block - no tosses to unblock
    					break;
    			}
    			else if (next_square_data.player_color == Definitions::NONE)
    			{
    				// maybe add rebirth
    				df_blocked &= ~((1<<j)-1);
    				df_moves += probs[j];
    			}
    		}

    	}
    	last_square = square_data.player_color;
    }

    // Maximal/minimal/0 value for finished game
    if (at_pieces==0) at_bonus += 100;
    if (df_pieces==0) df_bonus += 100;
    // Should add a move counter from draw scoring

    // We have:
    // at_pieces, at_dist, at_safe, at_exposed, at_blocked, (at_moves,) at_2block, at_3block
    // df_pieces, df_dist, df_safe, df_exposed, df_blocked, (df_moves,) df_2block, df_3block
    double at_chance_to_be_eaten=0, df_chance_to_be_eaten=0;
    double at_no_move_prob=0, df_no_move_prob=0;

    for (i=1; i<=m_num_sticks; i++)
    {
    	at_chance_to_be_eaten += ((at_exposed>>(i-1))&1)*probs[i];
    	df_chance_to_be_eaten += ((df_exposed>>(i-1))&1)*probs[i];
    	at_no_move_prob += ((at_blocked>>(i-1))&1)*probs[i];
    	df_no_move_prob += ((df_blocked>>(i-1))&1)*probs[i];
    }
    double at_score,df_score;
    if (at_third && df_third)
    {
		at_score = -at_dist*4 - at_pieces*10 + at_safe*4 + at_2block*8 + at_3block*20 - at_no_move_prob*14 - at_chance_to_be_eaten*16 + at_moves;
		df_score = -df_dist*4 - df_pieces*10 + df_safe*6 + df_2block*8 + df_3block*20 - df_no_move_prob*10 - df_chance_to_be_eaten*22 + df_moves;
    }
    else
    {
    	at_score = -at_dist*5 - at_pieces*10 + at_safe*10 + at_2block*8 + at_3block*20 - at_no_move_prob*14 - at_chance_to_be_eaten*16 + at_moves + (at_third?0:10) + at_bonus;
		df_score = -df_dist*5 - df_pieces*10 + df_safe*11 + df_2block*8 + df_3block*20 - df_no_move_prob*10 - df_chance_to_be_eaten*22 + df_moves + (df_third?0:10) + df_bonus;
    }

    if (attacker==playerColor)
    	return at_score-df_score;
    else
    	return df_score-at_score;
}

void SmartUtility::initialize(int size_squares, int num_pieces, int num_sticks)
{
	setParams(size_squares,num_pieces,num_sticks);

	if (probs.size()!=0)
		probs.clear();
	for (int i=0; i<=num_sticks; i++)
		probs.push_back(Combi::binom_prob(num_sticks,i));

	m_upperBound = size_squares*5*1.25+num_pieces*10+11*num_pieces+std::max(8*(num_pieces/2.),20*(num_pieces/3.))+14+22+num_pieces+10+100;//100000; //size_squares*num_pieces*4 + num_pieces*10 + 100;
	//              dist           pieces        safes         blocks                                        no_move,exposed,moves,third,bonus
	m_lowerBound = -m_upperBound;
}
