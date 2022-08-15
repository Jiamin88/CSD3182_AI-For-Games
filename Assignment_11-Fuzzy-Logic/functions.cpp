/*!*****************************************************************************
\file functions.cpp
\author Vadim Surov, Jee Jia Min
\par DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par Course: CS3182
\par Section: A
\par Programming Assignment 11
\date 07-21-2022
\brief
Implementation of the Fuzzy Logic classes that can be
used in development of games and simulations.
*******************************************************************************/

#include "functions.h"

namespace AI
{
	float MAX_VALUE = std::numeric_limits<float>::max();
	float MIN_VALUE = -MAX_VALUE;

	/**
	 * @brief 
	 * Calculates left shoulder and returns the degree of
	   membership for a particular value
	 * @param val 
	 * Value to calculate dom with.
	 * @return
	 * Range [0, 1]
	 * – 0 denoting that x is not a member
	 * – 1 denoting that x is definitely a member
	*/
	float FuzzySet_LeftShoulder::calculateDOM( float val ) const
	{
		if ( ( peakPoint == val ) || ( leftOffset == 0.0f ) || ( rightOffset == 0.0f ) )
			return 1.0f;

		if ( ( val >= peakPoint - leftOffset ) && ( val < peakPoint ) )
			return 1.0f;

		if ( ( val < ( peakPoint + rightOffset ) ) && ( val >= peakPoint ) )
			return 1.0f - ( ( val - peakPoint ) / rightOffset );

		return 0.0f;
	}

	/**
	 * @brief
	 * Calculates right shoulder and returns the degree of
	   membership for a particular value
	 * @param val
	 * Value to calculate dom with.
	 * @return
	 * Range [0, 1]
	 * – 0 denoting that x is not a member
	 * – 1 denoting that x is definitely a member
	*/
	float FuzzySet_RightShoulder::calculateDOM( float val ) const
	{
		if ( ( peakPoint == val ) || ( leftOffset == 0.0f ) || ( rightOffset == 0.0f ) )
			return 1.0f;

		if ( ( val > peakPoint - leftOffset ) && ( val <= peakPoint ) )
			return ( ( val - ( peakPoint - leftOffset ) ) / leftOffset );

		if ( ( val <= ( peakPoint + rightOffset ) ) && ( val > peakPoint ) )
			return 1.0f;

		return 0.0f;
	}

	/**
	 * @brief
	 * Calculates dom of center shoulder and returns
	   the degree of membership for a particular value
	 * @param val
	 * Value to calculate dom with.
	 * @return
	 * Range [0, 1]
	 * – 0 denoting that x is not a member
	 * – 1 denoting that x is definitely a member
	*/
	float FuzzySet_Singleton::calculateDOM( float val ) const
	{
		if ( ( val >= ( peakPoint - leftOffset ) ) &&
			 ( val <= ( peakPoint + rightOffset ) ) )
			return 1.0f;
		return 0.0f;
	}

	/**
	 * @brief 
	 * Calculates dom of triangle.
	 * @param val 
	 * Value to calculate dom with.
	 * @return 
	 * Range [0, 1]
	 * – 0 denoting that x is not a member
	 * – 1 denoting that x is definitely a member
	*/
	float FuzzySet_Triangle::calculateDOM( float val ) const
	{
		if ( val == peakPoint && ( leftOffset == 0.0f || rightOffset == 0.0f ) )
			return 1.0f;

		if ( val > peakPoint && val < ( peakPoint + rightOffset ) )
			return 1.0f - ( ( val - peakPoint ) / rightOffset );

		if ( val <= peakPoint && val >= ( peakPoint - leftOffset ) )
			return ( ( 1.0f / leftOffset ) * ( val - ( peakPoint - leftOffset ) ) );
		return 0.0f;
	}

	/**
	 * @brief 
	 * Gets the dom for FuzzyAND
	 * @return 
	 * Smallest dom value 
	*/
	float FuzzyAND::getDOM()
	{
		float smallest = MAX_VALUE;
		for ( std::shared_ptr<FuzzySet> set : sets )
		{
			float current = set->getDOM();
			smallest = current < smallest ? current : smallest;
		}
		return ( smallest == MAX_VALUE ? 0.0f : smallest );
	}

	/**
	 * @brief 
	 * Gets the dom for FuzzyOR
	 * @return 
	 * Largest dom value
	*/
	float FuzzyOR::getDOM()
	{
		float largest = MIN_VALUE;
		for ( std::shared_ptr<FuzzySet>set : sets )
		{
			float current = set->getDOM();
			largest = current > largest ? current : largest;
		}
		return ( largest == MIN_VALUE ? 0.0f : largest );
	}

	/**
	 * @brief 
	 * Defuzzifies the value by averaging the maxima of the sets that have fired.
	 * @return
	 * Returns sum (maxima * degree of membership) / sum (degree of memberships)
	*/
	float FuzzyVariable::deFuzzifyMaxAv()
	{
		float bottom{ 0.0f };
		float top{ 0.0f };

		for ( std::pair<std::string, std::shared_ptr<FuzzySet>> set : sets )
		{
			bottom += set.second->getDOM();
			top += set.second->getRepresentativeValue() * set.second->getDOM();
		}
		return ( ( bottom == 0.0f ) ? 0.0f : ( top / bottom ) );
	}

	/**
	 * @brief 
	 * Defuzzify the variable using the centroid method
	 * @param numSamples 
	 * Value to be defuzzed
	 * @return 
	 * Defuzzified value
	*/
	float FuzzyVariable::deFuzzifyCentroid( int numSamples )
	{
		float step_size{ ( maxRange - minRange ) / numSamples };
		float total_area{ 0.0f };
		float sum_of_moments{ 0.0f };

		for ( auto sample = 1; sample <= numSamples; ++sample )
		{
			for ( std::pair<std::string, std::shared_ptr<FuzzySet>> set : sets )
			{
				float contribution = std::min( set.second->calculateDOM( minRange + ( sample * step_size ) ), set.second->getDOM() );
				total_area += contribution;
				sum_of_moments += ( minRange + ( sample * step_size ) ) * contribution;
			}
		}
		return ( 0.0f == total_area ? 0.0f : ( sum_of_moments / total_area ) );
	}

	/**
	 * @brief 
	 * To be deffuzificated
	 * @param varName 
	 * Fuzzy variable
	 * @param method 
	 * Deffuzification method
	 * @return 
	 * Returns some crisp value
	*/
	float FuzzyModule::deFuzzify( const std::string& varName, DefuzzifyMethod method )
	{
		// Your code ...
		setConfidencesOfConsequentsToZero();

		for ( FuzzyRule& rule : rules )
			rule.calculate();

		switch ( method )
		{
		case FuzzyModule::DefuzzifyMethod::centroid:
			return variables[varName].deFuzzifyCentroid( numSamples );
			break;
		case FuzzyModule::DefuzzifyMethod::max_av:
			return variables[varName].deFuzzifyMaxAv();
			break;
		}
		return 0.0f;
	}
} // end namespace