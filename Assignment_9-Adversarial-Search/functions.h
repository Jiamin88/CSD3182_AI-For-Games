/*!*****************************************************************************
\file functions.cpp
\author Vadim Surov, Jee Jia Min
\par DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par Course: CSD3182
\par Section: A
\par Programming Assignment 9
\date 7-July-2022
\brief
Implement an adversarial search for a two player game like TicTac-Toe
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <cstring> // memcpy
#include <limits.h>

#include "data.h"

#define UNUSED(x) (void)x;

// Class that defines the game specific data and code
class Grid
{
	static const int width = 3;
	static const int height = 3;

	char squares[width * height];

public:
	// The game marks/pieces
	static const char x = 'x';
	static const char o = 'o';
	static const char _ = ' ';

	/**
	 * @brief 
	 * Constructor to create empty grid
	 * @param squares 
	 * Set all grids in square to underscore
	*/
	Grid( char* squares = nullptr )
		: squares{ _, _, _, _, _, _, _, _, _ }
	{
		if ( squares )
			for ( int i = 0; i < height * width; ++i )
				this->squares[i] = squares[i];
	}

	/**
	 * @brief 
	 * Copy constructor
	 * @param rhs 
	 * Move rhs datas to lhs
	*/
	Grid( const Grid& rhs )
	{
		this->operator=( rhs );
	}

	/**
	 * @brief 
	 * Overloading of = operator
	 * @param rhs 
	 * Move rhs datas to lhs
	*/
	void operator=( const Grid& rhs )
	{
		std::memcpy( squares, rhs.squares, height * width * sizeof( char ) );
	}

	/**
	 * @brief 
	 * Set particular grid to the charater
	 * @param i 
	 * Which grid to set ?
	 * @param c 
	 * What character to set ?
	*/
	void set( int i, char c )
	{
		squares[i] = c;
	}

	/**
	 * @brief 
	 * Clear a particular grid in the squares
	 * @param i 
	 * Which grid to clear ?
	*/
	void clear( int i )
	{
		squares[i] = _;
	}

	/**
	 * @brief 
	 * For example for grid [' ',' ','o',' ','x',' ',' ',' ',' ']
	 * the function returns [0,1,3,5,6,7,8]
	 * @return 
	 * Returns a list of indices of all empty squares in the grid.
	*/
	std::vector<int> emptyIndices() const
	{
		std::vector<int> array;
		for ( int i = 0; i < height * width; ++i )
		{
			if ( squares[i] == _ )
			{
				array.push_back( i );
			}
		}
		return array;
	}

	/**
	 * @brief 
	 * @param player 
	 * @return 
	 * Returns true if the grid has a winning configuration for the player
	*/
	bool winning( char player )
	{
		return
			( squares[0] == player && squares[1] == player && squares[2] == player ) ||
			( squares[3] == player && squares[4] == player && squares[5] == player ) ||
			( squares[6] == player && squares[7] == player && squares[8] == player ) ||
			( squares[0] == player && squares[3] == player && squares[6] == player ) ||
			( squares[1] == player && squares[4] == player && squares[7] == player ) ||
			( squares[2] == player && squares[5] == player && squares[8] == player ) ||
			( squares[0] == player && squares[4] == player && squares[8] == player ) ||
			( squares[2] == player && squares[4] == player && squares[6] == player );
	}

	/**
	 * @brief 
	 * Overloading of output stream (<<)
	 * @param os 
	 * Overload the insertion operator to recognize an ostream object on the left 
	 * @param rhs 
	 * Data to be extracted
	 * @return 
	 * Output stream
	*/
	friend std::ostream& operator<<( std::ostream& os, const Grid& rhs )
	{
		for ( int j = 0; j < rhs.height; ++j )
		{
			os << ( j == 0 ? '[' : ' ' );

			for ( int i = 0; i < rhs.width; ++i )
			{
				os << rhs.squares[( j * rhs.width ) + i] << ( ( i == 2 && j == 2 ) ? ']' : ',' );
			}
			if ( j != ( rhs.height - 1 ) )
			{
				os << '\n';
			}
		}

		return os;
	}
};

namespace AI
{
	// A node of the game tree
	template<typename T>
	class Move
	{
		T grid;			// Result of a move: new state of the game grid			
		int score;		// Score of the move			
		std::vector<Move*>* next;  // All possible next moves
		int bestMove;   // Index of the first move in member next that has the best score 
		int spotIndex;  // Index of the move's spot (used for a visualization)

	public:
		/**
		 * @brief 
		 * Constructor
		 * @param grid 
		 * @param score 
		 * @param next 
		 * @param bestMove 
		*/
		Move( T grid = {}, int score = 0, std::vector<Move*>* next = new std::vector<Move*>{}, int bestMove = -1 )
			: grid{ grid }, score{ score }, next{ next }, bestMove{ bestMove }, spotIndex{ -1 }
		{}

		/**
		* @brief 
		* Destructor to dealocated memory that has been newed
		*/
		~Move()
		{
			for ( auto& next_moves : *next )
			{
				delete next_moves;
			}
			delete next;
		}

		/**
		 * @brief 
		 * @param i 
		 * @return 
		 * 
		*/
		Move& at( int i )
		{
			return *( next->at( i ) );
		}

		/**
		 * @brief 
		 * @return 
		*/
		int getScore() const
		{
			return score;
		}

		/**
		 * @brief 
		 * Set index at particular spot
		 * @param i 
		 * index to be set
		*/
		void setSpotIndex( int i )
		{
			spotIndex = i;
		}

		/**
		 * @brief 
		 * Overloading of output stream operator (<<)
		 * @param os 
		 * Output stream
		 * @param rhs 
		 * Datas to be output
		 * @return 
		 * Overloaded output data in os stream
		*/
		friend std::ostream& operator<<( std::ostream& os, const Move& rhs )
		{
			os << rhs.grid << std::endl;
			os << rhs.score << std::endl;
			os << rhs.next->size() << std::endl;
			os << rhs.bestMove << std::endl;
			return os;
		}
	};

	/**
	 * @brief 
	 * Find the best next move for maximizer.
	 * For the initial call, set the player parameter as maximizer. @tparam T 
	 * Warning! Found solution (sequence of moves) is not necessary shortest.@param grid 
	 * @param player 
	 * Who the player is doesn't matter
	 * @param maximizer 
	 * Player 1
	 * @param minimizer 
	 * Player 2
	 * @return 
	 * If the game is over, return the score from X's perspective.
	*/
	template<typename T>
	Move<T>* minimax( T grid, char player, char maximizer, char minimizer )
	{
		// Checks for the terminal states such as win and lose 
		// and returning a value accordingly
		if ( grid.winning( minimizer ) )
			return new Move<T>( grid, -10 );
		else if ( grid.winning( maximizer ) )
			return new Move<T>( grid, 10 );


		std::vector<int> availSpots = grid.emptyIndices();
		// Checks for the terminal states tie 
		if ( availSpots.size() == 0 )
			return new Move<T>( grid, 0 );

		// An array of all next possible moves
		std::vector<Move<T>*>* next = new std::vector<Move<T>*>{};

		for ( int i = 0; i < static_cast< int >( availSpots.size() ); ++i )
		{
			// Set the empty spot as next move of the current player
			grid.set( availSpots[i], player );

			// Collect the score resulted from calling minimax 
			// on the opponent of the current player
			auto move = minimax
			(
				grid,
				( player == maximizer ) ? minimizer : maximizer,
				maximizer,
				minimizer
			);

			// Store the index of that spot 
			move->setSpotIndex( availSpots[i] );

			// Push the move object to the array
			next->push_back( move );

			// Reset the spot to empty
			grid.clear( availSpots[i] );
		}

		// If it is the maximizer turn, loop over the moves and 
		// choose the move with the highest score
		int bestScore = 0;
		int bestMove = -1;

		// If it is the maximizer turn, loop over the moves and 
		// choose the move with the highest score
		if ( player == maximizer )
		{
			bestScore = -( std::numeric_limits<int>::max )( );
			for ( int i = 0; i < static_cast< int >( next->size() ); ++i )
			{
				//std::vector<Move<T>*>* next;
				if ( next->at( i )->getScore() > bestScore )
				{
					bestScore = next->at( i )->getScore();
					bestMove = i;
				}
			}
		}
		else // Else, choose the move with the lowest score
		{
			bestScore = ( std::numeric_limits<int>::max )( );
			for ( int i = 0; i < static_cast< int >( next->size() ); ++i )
			{
				if ( next->at( i )->getScore() < bestScore )
				{
					bestScore = next->at( i )->getScore();
					bestMove = i;
				}
			}
		}
		// Return the move
		return new Move<T>( grid, bestScore, next, bestMove );
	}
} // end namespace

#endif