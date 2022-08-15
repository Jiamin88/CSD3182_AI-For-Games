/*!*****************************************************************************
\file functions.cpp
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

#include "functions.h"

namespace AI
{
    /**
     * @brief 
     * Some random generator
     * @param key 
     * Key used to generated
     * @return 
     * The generated result
    */
    int random( size_t key )
    {
        return rand() % key;
    }
} // end namespace