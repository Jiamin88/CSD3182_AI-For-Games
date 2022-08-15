/*!*****************************************************************************
\file	functions.h
\author	Vadim Surov, Jee Jia Min
\par	DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par	Course: CSD3182
\par	Section: A
\par	Programming Assignment 1
\date	15-05-2022
\brief
The traversal (or search) of the graph by using the two approaches,
one is the breadth-first search (BFS) and another one is depth-first search (DFS).
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <sstream>
#include <string>
#include <list>
#include <vector>
#include <queue>
#include <stack>
#include <algorithm>

#include "data.h"

namespace AI
{
#define UNUSED(expr) (void)expr;

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
			:
			value{ value },
			parent{ parent },
			children{ children }
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

	/**
	 * @brief
	 * Implementation of the Breadth-First Search algorithm
	 * @tparam T
	 * @param node
	 * node to be search though
	 * @param lookingfor
	 * data to be find
	 * @return
	*/
	template<typename T>
	Node<T>* BFS( Node<T>& node, const T& lookingfor )
	{
		std::queue<Node<T>*> bfsqueue;
		bfsqueue.push( &node );

		Node <T>* vertex{ nullptr };

		while ( !bfsqueue.empty() )
		{
			vertex = bfsqueue.front();
			bfsqueue.pop();
			if ( vertex->value == lookingfor )
				return vertex;
			for ( auto const& child : vertex->children )
				bfsqueue.push( child );
		}
		return nullptr;
	}

	/**
	 * @brief
	 * Breadth First Search Algorithm for Graph Implemented
	 * using C++ STL which using a Queue
	 * @tparam T
	 * @param node
	 * node to be search through
	 * @param lookingfor
	 * data to be find
	 * @return
	*/
	template<typename T>
	Node<T>* DFS( Node<T>& node, const T& lookingfor )
	{
		// stack for DFS 
		std::stack < Node <T>*> dfsstack;
		// current source node inside stack 
		dfsstack.push( &node );
		// temp holder for current vertex
		Node <T>* vertex{ nullptr };
		while ( !dfsstack.empty() )
		{
			// Pop a vertex  
			vertex = dfsstack.top();
			dfsstack.pop();
			// explore all adjacent vertices of popped vertex. 
			if ( vertex->value == lookingfor )
				return vertex;

			// push the vertex to the stack if still not visited
			for ( auto const& child : vertex->children )
				dfsstack.push( child );
		}
		return nullptr;
	}

} // end namespace

#endif