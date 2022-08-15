/*!*****************************************************************************
\file functions.h
\author Vadim Surov, Jee Jia Min
\par DP email: vsurov\@digipen.edu, j.jiamin\@digipen.edu
\par Course: CSD3182
\par Section: A
\par Programming Assignment 12
\date 07-31-2022
\brief
This file has declarations and definitions that are required to
implement the Genetic Algorithm to solve N-Bits problem and 8-
Queens Puzzle.
*******************************************************************************/

#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include <iostream>
#include <sstream>
#include <vector>
#include <cmath>

#include "data.h"

#define UNUSED(x) (void)x;

template<typename Gene>
struct Fitness_Accumulate
{
	/**
	 * @brief 
	 * Returns result of addition of all genes in a chromosome
	 * @param genes 
	 * Some genes
	*/
	int operator()( const std::vector<Gene>& genes ) const
	{
		UNUSED( genes );

		// Your code ...
		int sum{ 0 };
		for ( auto& gene : genes )
		{
			sum += gene.getValue();
		}
		return sum;
	}
};

template<typename Gene>
struct Fitness_Nbits
{
	/**
	 * @brief 
	 * Returns a measure in percentages that indicates the fitness of  
	 * a particular chromosome into a particular solution.
	 * Fittest chromosome has all genes equal 1.
	 * @param genes 
	 * Some genes
	*/
	int operator()( const std::vector<Gene>& genes ) const
	{
		UNUSED( genes );

		// Your code ...
		if ( genes.size() )
		{
			int sum = 0;
			for ( auto& gene : genes )
			{
				sum += gene.getValue();
			}
			return ( 100 * sum / ( int ) genes.size() );
		}
		return 0;
	}
};

template<typename Gene>
struct Fitness_8queens
{
	/**
	 * @brief 
	 * Returns a measure in percentages that indicates the fitness of  
	 * a particular chromosome into a particular solution of 8 queens 
	 * problem
	 * @param genes 
	 * Some genes
	*/
	int operator()( const std::vector<Gene>& genes ) const
	{
		UNUSED( genes );

		// Your code ...
		if ( genes.size() )
		{
			int size = ( int ) genes.size();
			int max = ( size - 1 ) * size / 2;
			int counter = 0;
			// Check horizontal threats
			for ( int j = 0; j < size - 1; ++j )
			{
				for ( int i = j + 1; i < size; ++i )
				{
					int dy = genes[j].getValue() - genes[i].getValue();
					if ( dy == 0 )
						counter++;
				}
			}
			// Check diagonal threats
			for ( int j = 0; j < size - 1; ++j )
			{
				for ( int i = j + 1; i < size; ++i )
				{
					int dx = i - j;
					int dy = std::abs( genes[j].getValue() - genes[i].getValue() );
					if ( dy == dx )
						counter++;
				}
			}
			return ( 100 * ( max - counter ) / max );
		}
		return 0;
	}
};


namespace AI
{

	int random( size_t key );

	// Crossover methods for the genetic algorithm
	enum CrossoverMethod
	{
		Middle, Random
	};

	// Simplest gene seeding class/function
	struct Seed
	{
		int operator()( int p = 0 ) const
		{
			return p;
		}
	};

	// Gene seeding class/function with a fixed value
	template<int Val = 0>
	struct Seed_Value
	{
		int operator()( int /* p */ = 0 ) const
		{
			return Val;
		}
	};

	template<int Max>
	struct Seed_Random
	{
		/**
		 * @brief 
		 * Gene random seeding class/function
		 * @return 
		 * Some seed
		*/
		int operator()( int /* p */ = 0 ) const
		{
			// Your code ...
			return ( int ) floor( rand() % Max );
		}
	};

	// Gene class
	template<typename T = int, typename S = Seed>
	class Gene
	{
		T value;

	public:
		Gene( int p = 0 )
			: value{ S()( p ) }
		{}

		T getValue() const
		{
			return value;
		}

		void setValue( T v )
		{
			value = v;
		}

		friend std::ostream& operator<<( std::ostream& os, const Gene& rhs )
		{
			os << rhs.value;
			return os;
		}
	};

	// Chromosome class
	template<typename Gene, typename Fitness, size_t Size>
	class Chromosome
	{
		std::vector<Gene> genes;
		int fitness;

	public:

		using gene_type = Gene;

		static const size_t size = Size;

		Chromosome()
			: genes( Size ), fitness{ Fitness()( genes ) }
		{
		}

		std::vector<Gene>& getGenes()
		{
			return genes;
		}

		void setGenes( const std::vector<Gene>& v )
		{
			genes = v;
			fitness = Fitness()( genes );
		}

		Gene getGene( size_t i ) const
		{
			return genes[i];
		}

		void setGene( size_t i, const Gene& v )
		{
			genes[i] = v;
			fitness = Fitness()( genes );
		}

		int getFitness() const
		{
			return fitness;
		}

		// Select a random mutation point and change 
		// gene at the mutation point
		void randomMutation()
		{
			setGene( std::rand() % Chromosome::size, Gene() );
		}

		// Copy genes from a source
		void copyGenesFrom( Chromosome& src )
		{
			std::copy( src.genes.begin(), src.genes.end(), genes.begin() );
			fitness = Fitness()( genes );
		}

		friend std::ostream& operator<<( std::ostream& os,
										 const Chromosome& rhs )
		{
			os << '[';
			for ( auto it = rhs.genes.begin(); it != rhs.genes.end(); ++it )
				os << *it << ( it + 1 != rhs.genes.end() ? "," : "" );
			os << "]=" << rhs.fitness;
			return os;
		}
	};

	// Individual class
	template<typename Chromosome>
	class Individual
	{
		Chromosome chromosome;

	public:

		using chromosome_type = Chromosome;
		using gene_type = typename Chromosome::gene_type;

		Individual()
			: chromosome{ }
		{}

		Chromosome& getChromosome()
		{
			return chromosome;
		}

		std::vector<gene_type>& getGenes()
		{
			return chromosome.getGenes();
		}

		void setGenes( const std::vector<gene_type>& v )
		{
			chromosome.setGenes( v );
		}

		gene_type getGene( size_t i ) const
		{
			return chromosome.getGene( i );
		}

		void copyGenesFrom( Individual& individual )
		{
			chromosome.copyGenesFrom( individual.chromosome );
		}

		void setGene( size_t i, gene_type gene )
		{
			chromosome.setGene( i, gene );
		}

		int getFitness() const
		{
			return chromosome.getFitness();
		}

		friend std::ostream& operator<<( std::ostream& os, Individual& rhs )
		{
			os << rhs.chromosome;
			return os;
		}
	};

	// Population class
	template<typename Individual>
	class Population
	{
		std::vector<Individual> individuals;
		Individual* fittest;

	public:
		Population( size_t size = 0 )
			: individuals{ }, fittest{ nullptr }
		{
			if ( size )
			{
				individuals.resize( size );
				updateFittest();
			}
		}

		size_t getSize() const
		{
			return individuals.size();
		}

		Individual& getIndividual( size_t i )
		{
			return individuals[i];
		}

		Individual* getFittest() const
		{
			return fittest;
		}

		/**
		 * @brief 
		 * Update some fittest
		*/
		void updateFittest()
		{
			// Your code ...
			if ( individuals.size() )
			{
				fittest = &individuals[0];
				for ( size_t i = 1; i < individuals.size(); ++i )
				{
					if ( individuals[i].getFitness() > fittest->getFitness() )
						fittest = &individuals[i];
				}
			}
			else
				fittest = nullptr;
		}

		friend std::ostream& operator<<( std::ostream& os, Population& rhs )
		{
			os << " = " << rhs.getFittest()->getFitness() << std::endl;
			for ( size_t i = 0; i < rhs.getSize(); ++i )
				os << "  " << i << ':' << rhs.getIndividual( i ) << std::endl;
			return os;
		}
	};

	// Genetic Algorithm class
	template<typename Individual>
	class GeneticAlgorithm
	{
		Population<Individual>* population;
		int generation;

	public:
		GeneticAlgorithm()
			: population{ nullptr }, generation{ 0 }
		{}

		/**
		 * @brief
		 * A destructor to deallocated memory
		*/
		~GeneticAlgorithm()
		{
			// Your code ...
			if ( population )
				delete population;
		}

		/**
		 * @brief 
		 * Return a pointer of the fittest individual
		*/
		Individual* getFittest() const
		{
			// Your code ...
			return population->getFittest();
		}

		/**
		 * @brief 
		 * Implementation of the Roulette Wheel Selection. The probability of an 
		 * individual to be selected is directly proportional to its fitness.
		 * @param sizeOfPopulation 
		 * what's the size of the population
		 * @return 
		 * Some population
		*/
		Population<Individual>* selection( size_t sizeOfPopulation )
		{
			UNUSED( sizeOfPopulation );

			// Your code ...
			if ( !population )
				setPopulation( new Population<Individual>( sizeOfPopulation ) );

			Population<Individual>* new_generation = new Population<Individual>( sizeOfPopulation );

			// Play roulette
			int sum_fitness = 0;
			for ( size_t i = 0; i < sizeOfPopulation; ++i )
			{
				sum_fitness += population->getIndividual( i ).getFitness();
			}
			for ( size_t i = 0; i < sizeOfPopulation; ++i )
			{
				auto rnd = random( sum_fitness );
				size_t I = 0;
				for ( ; I < sizeOfPopulation; ++I )
				{
					rnd -= population->getIndividual( I ).getFitness();
					if ( rnd <= 0 )
						break;
				}
				new_generation->getIndividual( i ).copyGenesFrom( population->getIndividual( I ) );
			}
			new_generation->updateFittest();
			return new_generation;
		}

		/**
		 * @brief 
		 * Some cross over parents gene happenning here
		 * @param newGeneration 
		 * The new gene 
		 * @param crossoverMethod 
		 * Whats the method to cross over
		*/
		void crossover( Population<Individual>* newGeneration,
						CrossoverMethod crossoverMethod )
		{
			UNUSED( newGeneration );
			UNUSED( crossoverMethod );

			// Your code ...

			size_t size_of_population = newGeneration->getSize();
			size_t size_of_chromosome = 8;
			size_t cross_over_point = 0;

			if ( crossoverMethod == CrossoverMethod::Middle )
				cross_over_point = size_of_chromosome / 2;
			else if ( crossoverMethod == CrossoverMethod::Random )
				cross_over_point = random( size_of_chromosome );

			for ( size_t j = 0; j < size_of_population - 1; j += 2 )
			{
				// Swap values among pairs
				for ( size_t i = 0; i < cross_over_point; ++i )
				{
					// Swap genes
					auto t = newGeneration->getIndividual( j + 1 ).getGene( i );
					newGeneration->getIndividual( j + 1 ).
						setGene( i, newGeneration->getIndividual( j ).getGene( i ) );
					newGeneration->getIndividual( j ).setGene( i, t );
				}
			}
		}

		/**
		 * @brief 
		 * Mutation of genes under a random probability]
		 * @param newGeneration 
		 * Some new gene
		 * @param mutationProbability 
		 * Some mutation probability
		*/
		void mutation( Population<Individual>* newGeneration,
					   int mutationProbability )
		{
			UNUSED( newGeneration );
			UNUSED( mutationProbability );

			// Your code ...
			auto size_of_population = newGeneration ?
				newGeneration->getSize() : 0;

			for ( size_t j = 0; j < size_of_population; ++j )
				// Select a random mutation point and flip 
				// genes at the mutation point
				if ( random( 100 ) < mutationProbability )
					newGeneration->getIndividual( j )
					.getChromosome().randomMutation();
		}

		/**
		 * @brief 
		 * Replace existing population if any with a new generation
		 * @param newGeneration 
		 * Some new gene
		*/
		void setPopulation( Population<Individual>* newGeneration )
		{
			UNUSED( newGeneration );

			// Your code ...
			delete population;
			population = newGeneration;
			population->updateFittest();
		}

		/**
		 * @brief 
		 * Start the search
		 * @param sizeOfPopulation 
		 * What's the size of the population
		 * @param mutationProbability 
		 * What's the mutation probability
		 * @param crossoverMethod 
		 * Some crossover method
		 * @param os 
		 * Output stream
		*/
		void run( size_t sizeOfPopulation = 100,
				  int mutationProbability = 70,
				  CrossoverMethod crossoverMethod = CrossoverMethod::Middle,
				  std::ostringstream* os = nullptr )
		{
			UNUSED( sizeOfPopulation );
			UNUSED( mutationProbability );
			UNUSED( crossoverMethod );
			UNUSED( os );

			// Your code ...
			generation = 0;
			( void ) os;
			setPopulation( new Population<Individual>( sizeOfPopulation ) );
			while ( next( sizeOfPopulation, mutationProbability, crossoverMethod, os ) )
			{
			}
		}

		/**
		 * @brief 
		 * Continue the search
		 * @param sizeOfPopulation
		 * What's the size of the population
		 * @param mutationProbability
		 * What's the mutation probability
		 * @param crossoverMethod
		 * Some crossover method
		 * @param os 
		 * Output stream
		 * @return 
		 * Some boolean
		*/
		bool next( size_t sizeOfPopulation,
				   int mutationProbability,
				   CrossoverMethod crossoverMethod,
				   std::ostringstream* os )
		{
			UNUSED( sizeOfPopulation );
			UNUSED( mutationProbability );
			UNUSED( crossoverMethod );
			UNUSED( os );

			// Your code ...
			( void ) os;

			if ( !population )
				setPopulation( new Population<Individual>( sizeOfPopulation ) );

			if ( population->getFittest()->getFitness() == 100 || generation > 10000 )
			{
				return false;
			}

			auto new_generation = selection( sizeOfPopulation );

			crossover( new_generation, crossoverMethod );
			mutation( new_generation, mutationProbability );
			setPopulation( new_generation );
			generation++;

			return true;
		}
	};

} // end namespace

#endif