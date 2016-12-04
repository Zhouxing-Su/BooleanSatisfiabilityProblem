////////////////////////////////
/// usage : 1.	input and output for the problem.
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SZX_BOOLEAN_SATISFIABILITY_PROBLEM_PROBLEM_H
#define SZX_BOOLEAN_SATISFIABILITY_PROBLEM_PROBLEM_H


#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <limits>

#include "Utility.h"


namespace szx {

// input for the problem.
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
    friend std::istream& operator>>(std::istream &is, Problem &problem) {
        char c;
        int v;
        std::string s;

        // clear comments.
        for (is >> c; c == 'c'; is >> c) { getline(is, s); }

        // read format.
        if (c == 'p') { is >> s >> problem.varNum >> problem.clauseNum; }
        problem.varIndexEnd = problem.varNum + 1;

        // read clauses.
        problem.literalNum = 0;
        problem.clauses.init(problem.clauseNum);
        for (auto clause = problem.clauses.begin(); is >> v; ++clause) {
            clause->push_back(v);
            for (is >> v; v != 0; is >> v) { clause->push_back(v); }
            problem.literalNum += static_cast<int>(clause->size());
        }

        return is;
    }

    void load(const std::string &path) {
        std::ifstream ifs(path);
        ifs >> *this;
    }
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


// measurement for the quality of different solutions.
class Objective {
    #pragma region Type
public:
    using Value = int;
    #pragma endregion Type

    #pragma region Constant
public:
    static constexpr Value MaxValue = std::numeric_limits<Value>::max() / 2; // avoid max+max overflow.
    static constexpr Value MinValue = std::numeric_limits<Value>::min();
    static constexpr Value DefaultValue = 0;
    #pragma endregion Constant

    #pragma region Constructor
public:
    Objective(Value objValue = DefaultValue) : value(objValue) {}
    Objective(const std::string &objDescription) : description(objDescription) {}
    Objective(Value objValue, const std::string &objDescription) : value(objValue), description(objDescription) {}

    friend std::ostream& operator<<(std::ostream &os, const Objective &obj) {
        return (os << obj.description << " " << obj.value);
    }
    #pragma endregion Constructor

    #pragma region Method
public:
    #pragma endregion Method

    #pragma region Field
public:
    Value value;
    Timer::TimePoint findTime;  // set by Solver::solve*() for output.
    std::string description;    // set by Solver::init() for output.
    #pragma endregion Field
}; // Objective


// output of the problem.
// VariableType could be double for relaxation.
template<typename VariableType = bool>
class Solution {
    #pragma region Type
public:
    using VarType = VariableType;
    #pragma endregion Type

    #pragma region Constant
public:
    #pragma endregion Constant

    #pragma region Constructor
public:
    Solution(int varIndexEnd) : obj(0), vars(varIndexEnd) {}
    Solution(const Problem &problem) : Solution<VariableType>(problem.varIndexEnd) {}

    friend std::ostream& operator<<(std::ostream &os, const Solution<VariableType> &sln) {
        os << sln.obj << endl;
        for (int x = Problem::VarIndexBegin; x < sln.vars.size(); ++x) {
            os << " " << (sln.vars.at(x) ? x : -x);
        }
        return os;
    }

    void save(const std::string &path) const {
        std::ofstream ofs(path);
        ofs << *this;
    }
    #pragma endregion Constructor

    #pragma region Method
public:
    #pragma endregion Method

    #pragma region Field
public:
    Objective obj;
    Arr<VarType> vars;
    #pragma endregion Field
}; // Solution

}


#endif // SZX_BOOLEAN_SATISFIABILITY_PROBLEM_PROBLEM_H
