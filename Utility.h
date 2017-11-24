/* Utility.h
 *
 */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Definitions.h"
#include "SenetBoard.h"

namespace senet_ai{

	/**
	 * An interface for a general utility that can be calculated on SenetBoards
	 */
	class Utility
	{
	public:
		// Constructor
		Utility() {};
		// Destructor
		// Does nothing
		virtual ~Utility()  {};

		// Reads the current board, get a current move,
		// Calculate utility function for suggested board
		virtual double calculate(const SenetBoard& curBoard, Definitions::PlayerColor playerColor) = 0;

		//sets the utility parameters
		void setParams(int size_squares, int num_pieces, int num_sticks);
		//initialize the utility
		virtual void initialize(int size_squares, int num_pieces, int num_sticks);
		//get upper bound for star searches (depends on the utility)
		double getUpperBound();
		//get lower bound for star searches (depends on the utility)
		double getLowerBound();
	
	protected:
		//the utility parameter
		int m_size_squares;
		int m_num_pieces;
		int m_num_sticks;
		double m_upperBound;
		double m_lowerBound;
	};
}

#endif
