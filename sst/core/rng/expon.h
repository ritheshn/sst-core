#ifndef _H_SST_CORE_RNG_EXP
#define _H_SST_CORE_RNG_EXP

#include "math.h"

#include "distrib.h"
#include "mersenne.h"

using namespace SST::RNG;

namespace SST {
namespace RNG {

/**
	Creates an exponential distribution for use within SST. This distribution is the same across
	platforms and compilers.
*/
class SSTExponentialDistribution : public SSTRandomDistribution {

	public:
		/**
			Creates an exponential distribution with a specific lambda
			\param lambda The lambda of the exponential distribution
		*/
		SSTExponentialDistribution(double lambda);

		/**
			Creates an exponential distribution with a specific lambda and a base random number generator
			\param lambda The lambda of the exponential distribution
			\param baseDist The base random number generator to take the distribution from.
		*/
		SSTExponentialDistribution(double lambda, SSTRandom* baseDist);

		/**
			Gets the next (random) double value in the distribution
			\return The next random double from the distribution
		*/
		virtual double getNextDouble();

		/**
			Gets the lambda with which the distribution was created
			\return The lambda which the user created the distribution with
		*/
		double getLambda();

	protected:
		double lambda;
		SSTRandom* baseDistrib;

};

}
}

#endif
