/*!*****************************************************************************
\file	functions.h
\author	Vadim Surov, Jee Jia Min
\par	DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par	Course: CSD3182
\par	Section: A
\par	Programming Assignment 4
\date	03-06-2022
\brief
Implement the Dijkstra's algorithms for path searching.
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "data.h"
#include <algorithm>

#define UNUSED(x) (void)x;

namespace AI
{
	// Domain specific functor that returns adjacent nodes
	class GetMapAdjacents : public GetAdjacents
	{
		int* map; // the map with integers where 0 means an empty cell
		int size; // width and hight of the map in elements

	public:

		GetMapAdjacents( int* map = nullptr, int size = 0 )
			: GetAdjacents(), map{ map }, size{ size }
		{}

		/**
		 * @brief 
		 * Find all the adjacent nodes based on the key given
		 * @return
		 * List of all adjacent nodes found 
		*/
		std::vector<AI::Node*> operator()( Key key )
		{
			std::vector<AI::Node*> list = {};

			if ( map )
			{
				if ( key[1] - 1 >= 0 && map[key[0] * size + key[1] - 1] != 1 )
				{
					AI::Node* west = new AI::Node( { key[0], key[1] - 1 }, 10, 'W' );
					list.push_back( west );
				}

				if ( key[1] + 1 < size && map[key[0] * size + key[1] + 1] != 1 )
				{
					AI::Node* east = new AI::Node( { key[0], key[1] + 1 }, 10, 'E' );
					list.push_back( east );
				}

				if ( key[0] - 1 >= 0 && map[( key[0] - 1 ) * size + key[1]] != 1 )
				{
					AI::Node* north = new AI::Node( { key[0] - 1 , key[1] }, 10, 'N' );
					list.push_back( north );
				}

				if ( key[0] + 1 < size && map[( key[0] + 1 ) * size + key[1]] != 1 )
				{
					AI::Node* south = new AI::Node( { key[0] + 1 , key[1] }, 10, 'S' );
					list.push_back( south );
				}
			}
			return list;
		}
	};

	class Dijkstras
	{
		GetAdjacents* pGetAdjacents;

	public:

		Dijkstras( GetAdjacents* pGetAdjacents )
			: pGetAdjacents( pGetAdjacents )
		{}

		// starting and target are arrays of 2 elements [j, i] that define positions on the map

		/**
		 * @brief
		 * Distance Hash Table:
		 We’ll need a table that maps each nodes total distance
		 from the starting node. The key will be the node and
		 the value will be its running total.

		 * Priority Queue:
		 When we compare all the possible routes we could take,
		 the priority queue will surface the smallest possible
		 route. In this way, we don’t have to scan an array
		 every time asking, which is the smallest.

		* Routes Hash Table:
		 We need a way to keep track of the current path
		 we’re building. The key will be the current node
		 we are at and its value will be the node it came from.

		 * @param starting
		 * From the first key 
		 * @param target
		 * To the last final destination key
		 * @return
		 * List of key info (N,S,E,W)
		 * 
		*/
		std::vector<char> run( Key starting, Key target )
		{
			HashTable ht{};		//close list
			PriorityQueue pq{};	//open list

			AI::Node* root = new AI::Node( starting );
			ht.add( root->key, root );
			for ( auto& data : pGetAdjacents->operator()( starting ) )
			{
				data->parent = root;
				data->g = root->g + 1;
				pq.push( data );
			}
			while ( !pq.empty() )
			{
				AI::Node* node = pq.pop();
				for ( auto& adjnode : pGetAdjacents->operator()( node->key ) )
				{
					if ( AI::Node* oldnode = ht.find( adjnode->key ) )
					{
						if ( oldnode->g > node->g + 1 )
						{
							oldnode->g = node->g + 1;
							oldnode->parent = node->parent;
						}
						delete adjnode;
					}
					else
					{
						adjnode->g = node->g + 1;
						adjnode->parent = node;
						pq.push( adjnode );
					}
				}
				ht.add( node->key, node );
			}

			// Implement the search
			if ( target[0] < 10 && target[1] > 0 )
				return getPath( ht.find( target ) );

			return getPath( nullptr );
		}

	private:

		/**
		 * @brief 
		 * Based on the pointer to the node ,
		 retrieve all the nodes info and 
		 return a container of characters 
		 * @param pNode
		 * Pointer to the nodes
		 * @return
		 * List of info to the nodes
		*/
		std::vector<char> getPath( Node* pNode )
		{
			std::vector<char> a{};
			// Trace back to return a vector of moves (.info)
			if ( pNode )
			{
				while ( pNode->parent )
				{
					a.push_back( pNode->info );
					pNode = pNode->parent;
				}
				std::reverse( a.begin(), a.end() );
			}
			return a;
		}
	};
} // end namespace
#endif