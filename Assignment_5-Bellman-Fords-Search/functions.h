/*!*****************************************************************************
\file functions.h
\author Vadim Surov, Jee Jia Min
\par DP email: vsurov\@digipen.edu,  j.jiamin\@digipen.edu
\par Course: CS380
\par Section: A
\par Programming Assignment 5
\date 06-09-2022
\brief
Implement the Bellman-Ford's algorithms for path searching on a graph with negative costs.
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <array>
#include <climits>
#include <algorithm>
#include <memory>

#include "data.h"

#define UNUSED(x) (void)x;

namespace AI
{
	const int null = -1;
	const int inf = INT_MAX;

	// An implementation of the Bellman-Ford algorithm. 
	// The algorithm finds the shortest path between a
	// starting node and all other nodes in the graph. 
	// The algorithm also detects negative cycles.
	template<int SIZE = 0>
	class BellmanFord
	{
	public:
		int* matrix; // the cost adjacency matrix
		int* distance;
		int* predecessor;

		/**
		 * @brief 
		 * This is the initialisation step, an array is created that
		 stores the distance of all vertices from the initial vertex.
		 * @param matrix 
		 * Initialised with the pointer to an array of int matrix.
		*/
		BellmanFord( int* matrix = nullptr )
			: matrix{ matrix }, distance{ nullptr }, predecessor{ nullptr }
		{
			predecessor = new int[SIZE];
			distance = new int[SIZE];
		}
		
		/**
		 * @brief
		 * Free all allocated pointer.
		*/
		~BellmanFord()
		{
			delete[] distance;
			delete[] predecessor;
		}

		/**
		 * @brief 
		 * Shortest distance between all vertices from the src.
		 * Negative Cycle : If there exists a path shorter than the regular edge traversal, there is a negative cycle.
		 * @param starting 
		 * @return 
		 * Returns false when negative cycles are found, and true otherwise.
		*/
		bool run( int starting = 0 )
		{
			if ( !matrix )
				return false;

			// Initialize predecessor array which will be used in shortest path
			// reconstruction after the algorithm has terminated.
			std::fill_n( predecessor, SIZE, null );
			// Initialize distances from src to all other vertices as INF
			std::fill_n( distance, SIZE, inf );
			distance[starting] = 0;

			for ( int k = 0; k < SIZE - 1; k++ )
			{
				int relaxations = 0;
				for ( int j = 0; j < SIZE; j++ )
				{
					for ( int i = 0; i < SIZE; i++ )
					{
						if ( ( j != i ) &&
							 ( distance[j] != inf ) &&
							 ( matrix[i + ( j * SIZE )] != inf ) &&
							 ( distance[j] + matrix[i + ( j * SIZE )] ) < distance[i] )
						{
							distance[i] = distance[j] + matrix[i + ( j * SIZE )];
							predecessor[i] = j;
							relaxations++;
						}
					}
				}
				// Stop when no more relaxation
				if ( relaxations == 0 ) 
					// There is no negative cycles 
					return true;
			}
			return false;
		}

		/**
		 * @brief 
		 * Interpretation of the result that reconstructs the
		   shortest path from starting point to target.
		 * @param target 
		 * @return 
		 * list of path in vector container.
		*/
		std::vector<int> getPath( int target )
		{
			std::vector<int> path{};

			while ( predecessor[target] != null )
			{
				path.push_back( target );
				target = predecessor[target];
			}
			std::reverse( path.begin(), path.end() );

			return path;
		}

		/**
		 * @brief 
		 * Interpretation of the result that create a route (step-by-step 
		   description) of the shortest path from start to end with cost
		 * @param target 
		 * @return 
		 * List of data in each array stored in vector container.
		*/
		std::vector<std::array<int, 3>> getRoute( int target )
		{
			std::vector<std::array<int, 3>> route{};
			for ( int i = target; predecessor[i] != null; i = predecessor[i] )
				route.push_back( { predecessor[i] ,i, distance[i] } );

			for ( int i = 0; i < ( static_cast< int >( route.size() ) - 1 ); ++i )
				route[i][2] -= route[i + 1][2];

			std::reverse( route.begin(), route.end() );
			return route;
		}

		/**
		 * @brief 
		 * A utility function used to print the solution
		 * @param os 
		 * Stream insertion << operators also can be overloaded
		   to perform input and output for user-defined types like an object.
		 * @param rhs 
		 * Data to be extracted into ostream;
		 * @return 
		 * Overloaded operator returns a reference to the original ostream object
		*/
		friend std::ostream& operator<<( std::ostream& os, const BellmanFord& rhs )
		{
			if ( !rhs.matrix )
			{
				os << "[] []";
				return os;
			}

			os << "[";
			for ( int i = 0; i < SIZE - 1; i++ )
			{
				if ( rhs.distance[i] == inf )
					os << "inf";
				else
					os << rhs.distance[i];
				os << ",";
			}

			if ( rhs.distance[SIZE - 1] == inf )
				os << "inf";
			else
				os << rhs.distance[SIZE - 1];

			os << "] ";

			os << "[";

			for ( int i = 0; i < SIZE - 1; i++ )
			{
				if ( rhs.predecessor[i] == null )
					os << "null";
				else
					os << rhs.predecessor[i];
				os << ",";
			}

			if ( rhs.predecessor[SIZE - 1] == null )
				os << "null";
			else
				os << rhs.predecessor[SIZE - 1];

			os << "]";
			return os;
		}
	};
} // end namespace

#endif