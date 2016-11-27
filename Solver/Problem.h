////////////////////////////////
/// usage : 1.	
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SZX_BOOLEAN_SATISFIABILITY_PROBLEM_PROBLEM_H
#define SZX_BOOLEAN_SATISFIABILITY_PROBLEM_PROBLEM_H


#include <iostream>
#include <vector>
#include <string>

#include "Utility.h"


namespace szx {
class Problem {
    #pragma region Type
public:
    #pragma endregion Type

    #pragma region Constant
public:
    static constexpr int VarIndexBegin = 1;
    #pragma endregion Constant

    #pragma region Constructor
public:
    friend std::istream& operator>>(std::istream &is, Problem &problem);

    void load(const std::string &path);
    #pragma endregion Constructor

    #pragma region Method
public:
    #pragma endregion Method

    #pragma region Field
public:
    int varNum;
    int varIndexEnd; // = (varNum + 1) to skip the dummy x_0.

    int clauseNum;
    int literalNum;
    Arr<std::vector<int>> clauses;
    #pragma endregion Field
}; // Problem

}


#endif // SZX_BOOLEAN_SATISFIABILITY_PROBLEM_PROBLEM_H
