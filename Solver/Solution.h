////////////////////////////////
/// usage : 1.	output for the problem.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SZX_SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLUTION_H
#define SZX_SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLUTION_H


#include <iostream>
#include <string>

#include "Problem.h"
#include "Utility.h"


namespace szx {
class Solution {
    #pragma region Type
public:
    using Objective = int;
    #pragma endregion Type

    #pragma region Constant
public:
    #pragma endregion Constant

    #pragma region Constructor
public:
    Solution(const Problem &problem) : unsatClauseNum(0), vars(problem.varIndexEnd) {}
    Solution(int varIndexBegin, int varIndexEnd) : unsatClauseNum(0), vars(varIndexEnd) {}

    friend std::ostream& operator<<(std::ostream &os, const Solution &sln);
    
    void save(const std::string &path) const;
    #pragma endregion Constructor

    #pragma region Method
public:
    #pragma endregion Method

    #pragma region Field
public:
    Objective unsatClauseNum;
    Arr<bool> vars;
    #pragma endregion Field
}; // Solution

}


#endif // SZX_SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLUTION_H
