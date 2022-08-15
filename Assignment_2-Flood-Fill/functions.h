/*!*****************************************************************************
\file	functions.h
\author	Vadim Surov, Jee Jia Min
\par	DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par	Course: CSD3182
\par	Section: A
\par	Programming Assignment 2
\date	22-05-2022
\brief
To perform a flood fill, consider the starting pixel, plus any
pixels connected 4-directionally to the starting pixel of the
same color as the starting pixel, plus any pixels connected
4-directionally to those pixels (also with the same color), and so on.
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <stack>
#include <queue>
#include <algorithm>

#include "data.h"

#define UNUSED(expr) (void)expr;

namespace AI
{
	// Domain specific functor that returns adjacent nodes
	class GetMapAdjacents : public GetAdjacents
	{
		int* map; // the map with integers where 0 means an empty cell
		int size; // width and hight of the map in elements

	public:

		/**
		 * @brief
		 * Constructor
		 * @param map
		 * Pointer to the 1D array map
		 * @param size
		 * Size of the map
		*/
		GetMapAdjacents( int* map = nullptr, int size = 0 )
			: GetAdjacents(), map{ map }, size{ size }
		{}

		/**
		 * @brief
		 * Destructor
		*/
		virtual ~GetMapAdjacents()
		{}

		std::vector<AI::Node*> operator()( Key key )
		{
			std::vector<AI::Node*> list = {};

			int di[] = { -1, 1, 0 ,0 }; //direction in vertical
			int dj[] = { 0, 0, -1 ,1 }; //direction in horizontal

			for ( int i = 0; i < 4; i++ )
			{
				int next_i = key.i + di[i];
				int next_j = key.j + dj[i];

				// Find and return all empty adjacent cells 
				if ( next_i >= 0 && next_j >= 0 && next_i < size && next_j < size && map[next_i + ( next_j * size )] != 1 )
				{
					AI::Node* data{ new AI::Node };
					data->key = { next_j , next_i };
					data->pValue = map + ( next_j + ( next_i * size ) );
					list.push_back( data );
				}
			}
			return list;
		}
	};

	// Domain specific functor that returns shuffled adjacent nodes
	class GetMapStochasticAdjacents : public GetMapAdjacents
	{
	public:

		/**
		 * @brief
		 * Constructor
		 * @param map
		 * Pointer to the 1D array map
		 * @param size
		 * Size of the map
		*/
		GetMapStochasticAdjacents( int* map, int size )
			: GetMapAdjacents{ map, size }
		{}

		std::vector<AI::Node*> operator()( Key key )
		{
			// Find and return all empty adjacent cells
			// Use the base class operator() and then shuffle the result
			auto shuffle_list = AI::GetMapAdjacents::operator()( key );
			std::random_shuffle( shuffle_list.begin(), shuffle_list.end() );
			return shuffle_list;
		}
	};

	// Wrappers that provide same interface for queue and stack
	struct Interface
	{
		virtual void clear() = 0;

		virtual void push( Node* pNode ) = 0;

		virtual Node* pop() = 0;
	};

	struct Queue : Interface //...
	{
		std::queue<Node*> queue;

		/**
		 * @brief
		 * Clear the queue.
		*/
		void clear()
		{
			while ( !queue.empty() )
			{
				queue.pop();
			}
		}

		/**
		 * @brief
		 * Inserts a new element at the end of the queue,
		 after its current last element. The content of
		 this new element is initialized to pNode.
		 * @param pNode
		 * Some node to be pushed.
		*/
		void push( Node* pNode )
		{
			queue.push( pNode );
		}

		/**
		 * @brief
		 * Removes the next element in the queue,
		 effectively reducing its size by one.
		 * @return
		 * Some node to be popped.
		*/
		Node* pop()
		{
			Node* pNode = queue.front();
			queue.pop();
			return pNode;
		}

		/**
		 * @brief
		 * Test whether container is empty
		 * @return
		 * Returns whether the queue is empty:
		 i.e. whether its size is zero.
		*/
		bool empty()
		{
			return queue.empty();
		}
	};

	struct Stack : Interface //...
	{
		std::stack < Node* > stack;

		/**
		 * @brief
		 * remove all the elements from a stack.
		*/
		void clear()
		{
			while ( !stack.empty() )
			{
				stack.pop();
			}
		}

		/**
		 * @brief
		 * push an element into the stack
		 * @param pNode
		 * element to be pushed
		*/
		void push( Node* pNode )
		{
			stack.push( pNode );
		}

		/**
		 * @brief
		 * pop an element from the stack
		 * @return
		 * some popped element
		*/
		Node* pop()
		{
			Node* pNode = stack.top();
			stack.pop();
			return pNode;
		}

		/**
		 * @brief
		 * used to check if the stack container is empty or not
		 * @return
		 * Returns whether the queue is empty
		*/
		bool empty()
		{
			return stack.empty();
		}
	};

	// Recursive Flood Fill
	class Flood_Fill_Recursive
	{
		GetAdjacents* pGetAdjacents;

	public:
		/**
		 * @brief
		 * constructor
		 * @param pGetAdjacents
		 * get pointer to adjacent nodes
		 *
		*/
		Flood_Fill_Recursive( GetAdjacents* pGetAdjacents )
			: pGetAdjacents{ pGetAdjacents }
		{}

		/**
		 * @brief
		 * recursive flood fill
		 * @param key
		 * key to starting point on the map
		 * @param color
		 * color to be filled
		*/
		void run( Key key, int color )
		{
			std::vector<AI::Node*> list = pGetAdjacents->operator()( key );
			// Implement the flood fill
			// loop through all the value in the adjacents nodes
			for ( auto& adjacent_nodes : list )
			{
				if ( *( adjacent_nodes->pValue ) == 0 )
				{
					// change the colour for the adjacents nodes
					*( adjacent_nodes->pValue ) = color;
					run( adjacent_nodes->key, color );
				}
			}
			for ( auto& child : list )
			{
				delete child;
			}
		}
	};

	// Iterative Flood Fill
	// Type T defines is it depth- or breadth-first
	template<typename T>
	class Flood_Fill_Iterative
	{
		GetAdjacents* pGetAdjacents;
		T openlist;

	public:
		/**
		 * @brief
		 * constructor
		 * @param pGetAdjacents
		 * get pointer to adjacent nodes
		 *
		*/
		Flood_Fill_Iterative( GetAdjacents* pGetAdjacents )
			: pGetAdjacents{ pGetAdjacents }, openlist{}
		{}

		/**
		 * @brief
		 * recursive flood fill
		 * @param key
		 * key to starting point on the map
		 * @param color
		 * color to be filled
		*/
		void run( Key key, int color )
		{
			openlist.clear();
			std::vector<AI::Node*> list = pGetAdjacents->operator()( key );
			for ( auto& child : list )
				openlist.push( child );

			while ( !openlist.empty() )
			{
				AI::Node* node = openlist.pop();
				// Implement the flood fill
				// loop through all the value in the adjacents nodes

				for ( auto& adjacent_nodes : pGetAdjacents->operator()( node->key ) )
				{
					if ( *( adjacent_nodes->pValue ) != color )
					{
						*( adjacent_nodes->pValue ) = color;
						openlist.push( adjacent_nodes );

					}
					else
						delete adjacent_nodes;
				}
				delete node;
			}
		};

	};
} // end namespace
#endif