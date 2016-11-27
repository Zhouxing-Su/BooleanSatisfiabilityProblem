////////////////////////////////
/// usage : 1.	
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLVER_H
#define SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLVER_H


#include "Problem.h"
#include "Solution.h"


namespace szx {
class Solver {
    #pragma region Type
public:
    class CmdInterface {
    public:
        static int run(int argc, char *argv[]);
    }; // CmdInterface

    #pragma endregion Type

    #pragma region Constant
public:
    #pragma endregion Constant

    #pragma region Constructor
public:
    Solver(const Problem &p) : problem(p), sln(p) {}
    #pragma endregion Constructor

    #pragma region Method
public:
    void solve() {
        solvePlainModel();
    }

    void solvePlainModel();
    #pragma endregion Method

    #pragma region Field
public:
private:
    Problem problem;
    Solution sln;
    #pragma endregion Field
}; // Solver

}


#endif // SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLVER_H
