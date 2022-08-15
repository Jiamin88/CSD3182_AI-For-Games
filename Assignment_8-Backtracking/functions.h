/*!*****************************************************************************
\file functions.h
\author Vadim Surov, Jee Jia Min
\par DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par Course: CSD3182
\par Section: A
\par Programming Assignment 8
\date 07-01-2022
\brief
Tmplement a solution searching algorithm with a backtracking for a
constraint satisfaction problems. Sudoku game is an example of such
games when this method is used.
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <stack>
#include <algorithm>
#include <vector>

#include "data.h"

#define UNUSED(x) (void)x;

// This is a domain specific functor class that is used to find a solution 
// for a one-dimensional Sudoku by using Backtracking algorithm.
// Functor returns next location which is not occupied yet on the map.
// If no location found, returns Location<>{ nullptr, 0 }.
class NextLocation_Sudoku1D
{
	AI::MapInt1D* map;

public:

	NextLocation_Sudoku1D( void* map )
		: map{ static_cast< AI::MapInt1D* >( map ) }
	{}

	AI::Location<> operator()() const
	{
		for ( int i = 0; i < map->size; ++i )
		{
			if ( map->base[i] == 0 )
				return AI::Location<int>{ map->base, i };
		}
		return AI::Location<int>{ nullptr, 0 };
	}
};

// This is a domain specific functor class that is used to find a solution 
// for a two-dimensional Sudoku by using Backtracking algorithm.
// Functor returns next location which is not occupied yet on the map.
// If no location found, returns Location<>{ nullptr, 0 }.
class NextLocation_Sudoku2D
{
	AI::MapInt2D* map;

public:

	NextLocation_Sudoku2D( void* map )
		: map{ static_cast< AI::MapInt2D* >( map ) }
	{}

	AI::Location<> operator()() const
	{
		for ( int j = 0; j < map->height; j++ )
		{
			for ( int i = 0; i < map->width; i++ )
			{
				int index = ( j * map->width ) + i;
				if ( map->base[index] == 0 )
				{
					return AI::Location<>{ map->base, index };
				}
			}
		}

		return AI::Location<>{ nullptr, 0 };
	}
};

// This is a domain specific functor class that is used to find a solution 
// for a one-dimensional Sudoku by using Backtracking algorithm.
// Functor returns next candidate for a specified location on the map.
// If no candidate found, returns 0.
class NextCandidate_Sudoku1D
{
	AI::MapInt1D* map;

public:

	NextCandidate_Sudoku1D( void* map )
		: map{ static_cast< AI::MapInt1D* >( map ) }
	{}

	int operator()( AI::Location<> location )
	{
		auto value = map->base[location.getIndex()];
		while ( value < map->size )
		{
			value++;
			bool found{ false };
			for ( int i = 0; i < map->size; i++ )
			{
				if ( map->base[i] == value )
				{
					found = true;
					break;
				}
			}
			if ( !found )
				return value;

		}
		return 0;

	}
};

// This is a domain specific functor class that is used to find a solution 
// for a two-dimensional Sudoku by using Backtracking algorithm.
// Functor returns next candidate for a specified location on the map.
// If no candidate found, returns 0.
class NextCandidate_Sudoku2D
{
	AI::MapInt2D* map;

public:

	NextCandidate_Sudoku2D( void* map )
		: map{ static_cast< AI::MapInt2D* >( map ) }
	{}

	int operator()( AI::Location<> location )
	{
		std::vector<bool>set( 10, false );

		int x = location.getIndex() % map->width;
		int y = location.getIndex() / map->width;

		for ( int j = 0; j < map->height; j++ )
		{
			int index = ( j * map->width ) + x;
			if ( map->base[index] != 0 )
				set[map->base[index]] = true;
		}

		for ( int i = 0; i < map->width; i++ )
		{
			int index = ( y * map->width ) + i;
			if ( map->base[index] != 0 )
				set[map->base[index]] = true;
		}

		int bigx = ( x / 3 ) * 3;
		int bigy = ( y / 3 ) * 3;
		for ( int j = 0; j < 3; j++ )
		{
			for ( int i = 0; i < 3; i++ )
			{
				int index = ( bigy + j ) * map->width + ( bigx + i );
				if ( map->base[index] != 0 )
					set[map->base[index]] = true;
			}
		}

		for ( int i = location.getValue(); i < map->width; ++i )
		{
			if ( !set[i + 1] )
				return i + 1;
		}
		return 0;
	}
};


namespace AI
{
	// Template class of the Backtracking algorithm.
	// Parameter NL defines domain-specific NextLocation functor. 
	// Parameter NC defines domain-specific NextCandidate functor. 
	template<typename NL, typename NC>
	class Backtracking
	{
		std::stack<Location<>> stack;
		NL next_location;
		NC next_candidate;

	public:
		Backtracking( void* map = nullptr )
			: next_location{ map }, next_candidate{ map }
		{}

		// Find solution in a blocking mode
		void run()
		{
			auto location = next_location();
			if ( location.notFound() )
			{
				return;
			}
			stack.push( location );
			while ( true )
			{
				auto stack_top = stack.top();

				if ( stack_top.notFound() )
				{
					return;
				}

				auto candidate = next_candidate( stack_top );
				if ( candidate )
				{
					stack_top.setValue( candidate );
					auto next_loc = next_location();
					if ( next_loc.notFound() )
					{
						return;
					}
					stack.push( next_loc );
				}
				else
				{
					stack.top().clearValue();
					stack.pop();
				}
			}
		}

		// One iteration of the search. Used by run() in a blocking running mode
		// or can be called by timer in an non-blocking run
		bool solve()
		{
			// Your code ...

			return false;
		}
	};

} // end namespace

#endif