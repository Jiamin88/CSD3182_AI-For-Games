/*!*****************************************************************************
\file functions.h
\author Vadim Surov, Jee Jia Min
\par DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par Course: CSD3182
\par Section: A
\par Programming Assignment 10
\date 16-07-2022
\brief
Implement a few components that can be used for construction of a behavior tree.
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <sstream>
#include <string>
#include <list>

#include "data.h"

#define UNUSED(x) (void)x;

namespace AI
{
	// Check the state of a task comparing it with given by parameter 
	class CheckState : public Task
	{
		Task checktask;
		State checkstate;

	public:
		CheckState( Task checktask = {}, State checkstate = State::Success )
			: Task{ "CheckState" }, checktask{ checktask }, checkstate{ checkstate }
		{}

		/**
		 * @brief
		 * Output level datas into log.
		 * @param log
		 * Stream insertion operator (<<).
		 * @param level
		 * String to be output.
		 * @return
		 * Returns an ostream reference.
		*/
		Task& operator()( Log* log = nullptr, std::string level = "" )
		{
			if ( log )
			{
				*log << level << "CheckState" << '(' << checktask.getId() << ',' << STATES[checkstate] << ')' << std::endl;
				state = checktask.getState() == checkstate ? State::Success : State::Failure;
				*log << level << "L " << STATES[state] << std::endl;
			}
			return *this;
		}
	};

	// Selector composite
	// Returns immediately with a success status code 
	// when one of its children runs successfully.
	class Selector : public Task
	{
		std::list<SMART> tasks;

	public:
		Selector( std::initializer_list<SMART> tasks = {} )
			: Task{ "Selector" }, tasks{ tasks }
		{
		}

		/**
		 * @brief
		 * Output level datas into log.
		 * @param log
		 * Stream insertion operator (<<).
		 * @param level
		 * String to be output.
		 * @return
		 * Returns an ostream reference.
		*/
		Task& operator()( Log* log = nullptr, std::string level = "" )
		{
			if ( log )
			{
				*log << level << "Selector()" << std::endl;
				state = State::Failure;
				for ( auto it = tasks.begin(); it != tasks.end(); ++it )
				{
					state = it->get()->operator()( log, ( level + "| " ) ).getState();
					if ( state != State::Failure )
						break;
				}
				*log << level << "L " << STATES[state] << std::endl;
			}
			return *this;
		}
	};

	// Sequence composite
	// Returns immediately with a failure status code 
	// when one of its children fails.
	class Sequence : public Task
	{
		std::list<SMART> tasks;

	public:
		Sequence( std::initializer_list<SMART> tasks = {} )
			: Task{ "Sequence" }, tasks{ tasks }
		{
		}

		/**
		 * @brief
		 * Output level datas into log.
		 * @param log
		 * Stream insertion operator (<<).
		 * @param level
		 * String to be output.
		 * @return
		 * Returns an ostream reference.
		*/
		Task& operator()( Log* log = nullptr, std::string level = "" )
		{
			if ( log )
			{
				*log << level << "Sequence()" << std::endl;
				state = State::Success;
				for ( auto it = tasks.begin(); it != tasks.end(); ++it )
				{
					state = it->get()->operator()( log, ( level + "| " ) ).getState();
					if ( state != State::Success )
						break;
				}
				*log << level << "L " << STATES[state] << std::endl;
			}
			return *this;
		}
	};

	// Random selector composite
	// Tries a single child at random.
	class RandomSelector : public Task
	{
		std::list<SMART> tasks;

	public:
		RandomSelector( std::initializer_list<SMART> tasks = {} )
			: Task{ "RandomSelector" }, tasks{ tasks }
		{
		}

		/**
		 * @brief
		 * Output level datas into log.
		 * @param log
		 * Stream insertion operator (<<).
		 * @param level
		 * String to be output.
		 * @return
		 * Returns an ostream reference.
		*/
		Task& operator()( Log* log = nullptr, std::string level = "" )
		{
			if ( log )
			{
				*log << level << "RandomSelector()" << std::endl;
				state = State::Success;
				state = tasks.begin()->get()->operator()( log, ( level + "| " ) ).getState();
				*log << level << "L " << STATES[state] << std::endl;
			}
			return *this;
		}
	};


	// Decorators 

	// Inverter
	//     Invert the value returned by a task
	class Inverter : public Task
	{
		SMART task;

	public:
		Inverter( SMART task = {} )
			: Task{ "Inverter" }, task{ task }
		{
		}

		/**
		 * @brief
		 * Output level datas into log.
		 * @param log
		 * Stream insertion operator (<<).
		 * @param level
		 * String to be output.
		 * @return
		 * Returns an ostream reference.
		*/
		Task& operator()( Log* log = nullptr, std::string level = "" )
		{
			if ( log )
			{
				*log << level << "Inverter()" << std::endl;
				state = State::Success;
				state = ( *task ).operator()( log, ( level + "| " ) ).getState();
				state = state == State::Success ? State::Failure : State::Success;
				*log << level << "L " << STATES[state] << std::endl;
			}
			return *this;
		}
	};

	//	Succeeder
	//	Always return success, irrespective of what the child node actually returned.
	//	These are useful in cases where you want to process a branch of a tree 
	//	where a failure is expected or anticipated, but you don’t want to 
	//	abandon processing of a sequence that branch sits on.
	class Succeeder : public Task
	{
		SMART task;

	public:
		Succeeder( SMART task = {} )
			: Task{ "Succeeder" }, task{ task }
		{
		}

		/**
		 * @brief
		 * Output level datas into log.
		 * @param log
		 * Stream insertion operator (<<).
		 * @param level
		 * String to be output.
		 * @return
		 * Returns an ostream reference.
		*/
		Task& operator()( Log* log = nullptr, std::string level = "" )
		{
			if ( log )
			{
				*log << level << "Succeeder()" << std::endl;
				state = State::Success;
				state = ( *task ).operator()( log, ( level + "| " ) ).getState();
				*log << level << "L " << STATES[State::Success] << std::endl;
			}
			return *this;
		}
	};

	// Repeater
	// A repeater will reprocess its child node each time its 
	// child returns a result. These are often used at the very 
	// base of the tree, to make the tree to run continuously. 
	// Repeaters may optionally run their children a set number of 
	// times before returning to their parent.
	class Repeater : public Task
	{
		SMART task;
		int counter;

	public:
		Repeater( SMART task = {}, int counter = 0 )
			: Task{ "Repeater" }, task{ task }, counter{ counter }
		{
		}

		/**
		 * @brief
		 * Output level datas into log.
		 * @param log
		 * Stream insertion operator (<<).
		 * @param level
		 * String to be output.
		 * @return
		 * Returns an ostream reference.
		*/
		Task& operator()( Log* log = nullptr, std::string level = "" )
		{
			if ( log )
			{
				*log << level << "Repeater(" << counter << ')' << std::endl;
				state = State::Success;
				if ( task && counter )
				{
					for ( int i = 0; i < counter; ++i )
						state = ( *task ).operator()( log, ( level + "| " ) ).getState();
				}
				*log << level << "L " << STATES[State::Success] << std::endl;
			}
			return *this;
		}
	};

	// Repeat_until_fail
	// Like a repeater, these decorators will continue to 
	// reprocess their child. That is until the child finally 
	// returns a failure, at which point the repeater will 
	// return success to its parent.
	class Repeat_until_fail : public Task
	{
		SMART task;

	public:
		Repeat_until_fail( SMART task = {} )
			: Task{ "Repeat_until_fail" }, task{ task }
		{
		}

		/**
		 * @brief
		 * Output level datas into log.
		 * @param log
		 * Stream insertion operator (<<).
		 * @param level
		 * String to be output.
		 * @return
		 * Returns an ostream reference.
		*/
		Task& operator()( Log* log = nullptr, std::string level = "" )
		{
			if ( log )
			{
				*log << level << "Repeat_until_fail()" << std::endl;
				state = State::Success;
				if ( task )
					state = ( *task ).operator()( log, ( level + "| " ) ).getState();
				*log << level << "L " << STATES[State::Success] << std::endl;
			}
			return *this;
		}
	};
} // end namespace

#endif