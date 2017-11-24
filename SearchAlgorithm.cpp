/*
 * SearchAlgorithm.cpp
 *
 *  Created on: Dec 18, 2009
 *      Author: shaharch
 */

/*
 * SearchAlgorithm.h
 *
 *  Created on: Dec 17, 2009
 *      Author: shaharch
 */

#include "SearchAlgorithm.h"
#include "Combi.h"

using namespace senet_ai;


//initiate the search variable and the probability vector
void SearchAlgorithm::initSearch(Utility *utility, Utility *lightUtil, Definitions::PlayerColor color, int num_sticks)
{
	the_color = color;
	m_utility = utility;
	m_lightUtil = lightUtil;
	m_num_sticks = num_sticks;
	for (int i=0; i<=m_num_sticks; i++)
		probs.push_back(Combi::binom_prob(m_num_sticks,i));
}

