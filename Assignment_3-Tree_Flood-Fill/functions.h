/*!*****************************************************************************
\file	functions.h
\author	Vadim Surov, Jee Jia Min
\par	DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par	Course: CSD3182
\par	Section: A
\par	Programming Assignment 3
\date	29-05-2022
\brief

*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <vector>
#include <list>
#include <stack>
#include <string>
#include <algorithm>
#include <queue>

#include "data.h"

#define UNUSED(x) (void)x;

namespace AI
{
	// A simple graph node definition with serialization functions
	template<typename T>
	struct Node
	{
		// Member data
		T value;
		Node* parent;
		std::list<Node*> children;

		/**
		 * @brief
		 * Constructor for node
		 * @param value
		 * value to be stored in node
		 * @param parent
		 * pointer to the parent
		 * @param children
		 * list of all pointer to the child
		*/
		Node( T value = {}, Node* parent = nullptr,
			  const std::list<Node*>& children = {} )
			: value{ value }, parent{ parent }, children{ children }
		{}

		/**
		* @brief
		* Destructor
		*/
		~Node()
		{
			for ( auto child : children )
				delete child;
		}

		/**
		 * @brief
		 * Serialization
		 * @param os
		 * output stream
		 * @param rhs
		 * data to be output
		 * @return
		 * write obj to stream
		*/
		friend std::ostream& operator<<( std::ostream& os, Node const& rhs )
		{
			os << rhs.value; // Just for test, you can change it
			os << " {";
			os << rhs.children.size() << " ";
			for ( auto const& child : rhs.children )
				os << *child;
			os << "} ";

			return os;
		}

		/**
		 * @brief
		 * Deserialization
		 * @param is
		 * input stream
		 * @param rhs
		 * data to be input
		 * @return
		 * read obj from stream
		*/
		friend std::istream& operator>>( std::istream& is, Node& rhs )
		{
			if ( is.rdbuf()->in_avail() )
			{
				is >> rhs.value;

				char character;
				is >> character;

				int number_of_children;
				is >> number_of_children;
				for ( int i = 0; i < number_of_children; ++i )
				{
					rhs.children.push_back( new Node() );
					Node& child = *( rhs.children.back() );
					child.parent = &rhs;
					is >> child;
				}

				is >> character;
			}
			return is;
		}

		/**
		 * @brief
		 * get path in vector container
		 * @return
		 * Returns values from root to this node as an array
		*/
		std::vector<T> getPath() const
		{
			std::vector<T> r;
			Node const* current = this;
			while ( current )
			{
				r.push_back( current->value );
				current = current->parent;
			}
			std::reverse( r.begin(), r.end() );
			return r;
		}
	};

	// The actual node type for this assignment
	using TreeNode = Node<std::string>;

	// Abstract base class for domain specific functors that return adjacent nodes
	class GetAdjacents
	{
	public:

		virtual ~GetAdjacents()
		{}

		virtual std::vector<TreeNode*> operator()( TreeNode* pNode ) = 0;
	};

	// Domain specific functor that returns adjacent nodes
	class GetTreeAdjacents : public GetAdjacents
	{
	public:

		GetTreeAdjacents()
			: GetAdjacents()
		{}

		/**
		 * @brief 
		 * get all adjacents data
		 * @param pNode 
		 * pointer to the adjacent node to be searched
		 * @return 
		 * vector to all adjacent nodes
		*/
		std::vector<TreeNode*> operator()( TreeNode* pNode )
		{
			// Push to the list all children of pNode excluding 
			// those with value not equal "x"
			std::vector<AI::TreeNode*> list = {};

			for ( auto& node : pNode->children )
			{
				if ( node->value == "x" )
				{
					list.push_back( node );
				}
			}
			return list;
		}
	};

	// Domain specific functor that returns shuffled adjacent nodes
	class GetTreeStochasticAdjacents : public GetTreeAdjacents
	{
	public:

		GetTreeStochasticAdjacents()
			: GetTreeAdjacents{}
		{}

		std::vector<TreeNode*> operator()( TreeNode* pNode )
		{
			auto shuffle_list = GetTreeAdjacents::operator()( pNode );
			std::random_shuffle( shuffle_list.begin(), shuffle_list.end() );

			// Use the base class operator() and then shuffle the result

			return shuffle_list;
		}
	};

	// Wrappers that provide same interface for queue and stack
	struct Interface
	{
		virtual void clear() = 0;

		virtual void push( TreeNode* pNode ) = 0;

		virtual TreeNode* pop() = 0;
	};

	struct Queue : Interface //...
	{
		std::queue<TreeNode*> queue;

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
		void push( TreeNode* pNode )
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
		TreeNode* pop()
		{
			TreeNode* pNode = queue.front();
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
		std::stack < TreeNode* > stack;

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
		void push( TreeNode* pNode )
		{
			stack.push( pNode );
		}

		/**
		 * @brief
		 * pop an element from the stack
		 * @return
		 * some popped element
		*/
		TreeNode* pop()
		{
			TreeNode* pNode = stack.top();
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
		GetTreeAdjacents* pGetAdjacents;

	public:
		/**
		 * @brief
		 * constructor
		 * @param pGetAdjacents
		 * get pointer to adjacent nodes
		 *
		*/
		Flood_Fill_Recursive( GetTreeAdjacents* pGetAdjacents )
			: pGetAdjacents{ pGetAdjacents }
		{}

		/**
		 * @brief
		 * recursive flood fill
		 * @param pNode
		 * key to the first pointer to the tree
		 * @param value
		 * velue to be replaced or filled
		*/
		void run( TreeNode* pNode, std::string value )
		{
			// Implement the flood fill
			std::vector<TreeNode*> list = pGetAdjacents->operator()( pNode );
			// loop through all the value in the adjacents nodes
			for ( auto& adjacent_nodes : list )
			{
				if ( adjacent_nodes->value != value )
				{
					// change the colour for the adjacents nodes
					adjacent_nodes->value = value;
					run( adjacent_nodes, value );
				}
			}
		}
	};

	// Iterative Flood Fill
	// Type T defines is it depth- or breadth-first
	template<typename T>
	class Flood_Fill_Iterative
	{
		GetTreeAdjacents* pGetAdjacents;
		T openlist;

	public:
		/**
		 * @brief
		 * constructor
		 * @param pGetAdjacents
		 * get pointer to adjacent nodes
		 *
		*/
		Flood_Fill_Iterative( GetTreeAdjacents* pGetAdjacents )
			: pGetAdjacents{ pGetAdjacents }, openlist{}
		{
		}

		/**
		 * @brief 
		 * recursive flood fill
		 * @param pNode 
		 * key to the first pointer to the tree
		 * @param value 
		 * velue to be replaced or filled
		*/
		void run( TreeNode* pNode, std::string value )
		{
			openlist.clear();
			std::vector<TreeNode*> list = pGetAdjacents->operator()( pNode );
			for ( auto& child : list )
			{
				if ( child->value != value )
				{
					child->value = value;
					openlist.push( child );
				}
			}

			while ( !openlist.empty() )
			{
				TreeNode* node = openlist.pop();
				// Implement the flood fill
				// loop through all the value in the adjacents nodes

				for ( auto& adjacent_nodes : pGetAdjacents->operator()( node ) )
				{
					if ( adjacent_nodes->value != value )
					{
						adjacent_nodes->value = value;
						openlist.push( adjacent_nodes );
					}
				}
			}
		}
	};
} // end namespace

#endif