/*
 * Combi.h
 *
 *  Created on: Dec 27, 2009
 *      Author: shaharch
 */

#ifndef COMBI_H_
#define COMBI_H_

namespace senet_ai{
	/**
	 * simple header to help with combinatorics calculations
	 */
	struct Combi {
	public:
		Combi() {}
		~Combi() {}
		static int factorial(int n)
		{
			int res = 1;
			for (int i=2; i<=n; i++)
				res *= i;
			return res;
		}
		static int nchoosek(int n, int k) { return (factorial(n)/factorial(k))/factorial(n-k); }
		static double binom_prob(int n, int i)	{ return double(nchoosek(n,i))/(1<<n); }
	};
}

#endif /* COMBI_H_ */
