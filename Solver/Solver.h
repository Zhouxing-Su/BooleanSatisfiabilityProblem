////////////////////////////////
/// usage : 1.	
/// 
/// note  : 1.	
////////////////////////////////

#ifndef SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLVER_H
#define SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLVER_H


#include <iostream>
#include <string>

#include "Problem.h"
#include "Utility.h"


namespace szx {

class Solver {
    #pragma region Type
public:
    using Iteration = int;

    struct CmdInterface {
        static int run(int argc, char *argv[]);
    }; // CmdInterface

    struct CheckStatus {
        friend std::ostream& operator<<(std::ostream &os, const CheckStatus &checkStatus) {
            return (os << "feasible=" << checkStatus.feasible << " objMatch=" << checkStatus.objMatch);
            // EXTEND[szx][9]: report the occurrences of violation or mismatch.
        }

        bool checked;

        bool feasible;
        bool objMatch;

        Objective::Value objVal;
    }; // CheckStatus

    struct SolveStatus {
        SolveStatus(Timer::Duration timeoutInSecond)
            : timer(static_cast<Timer::TickCount>(timeoutInSecond * Timer::MillisecondsPerSecond)) {}

        Timer timer;
        Iteration iteration;
        Iteration generation;
    }; // SolveStatus

    // specify input/output path, termination conditions,
    // random seed and extra information for a solver.
    struct Environment {
        struct Key {
            static constexpr auto InstancePath = "InstancePath";
            static constexpr auto SolutionPath = "SolutionPath";
            static constexpr auto RandSeed = "RandSeed";
            static constexpr auto TimeoutInSecond = "TimeoutInSecond";
            static constexpr auto MaxIter = "MaxIter";
            static constexpr auto ThreadNum = "ThreadNum";
            static constexpr auto CfgPath = "CfgPath";
            static constexpr auto LogPath = "LogPath";
            static constexpr auto Id = "Id";
        };

        static constexpr int DefaultTimeout = (1 << 30);
        static constexpr int DefaultMaxIter = (1 << 30);
        static constexpr int DefaultThreadNum = 0;


        // set optional fields with default value.
        Environment() : timeoutInSecond(DefaultTimeout), randSeed(RandSeed::generate()),
            maxIter(DefaultMaxIter), threadNum(DefaultThreadNum) {}


        // set fields which are given in file, leave them untouched otherwise.
        void loadFromFile(const std::string &path);
        // save all fields to file.
        void saveToFile(const std::string &path) const;


        /// essential information.
        std::string instancePath;
        std::string solutionPath;

        /// optional information. it is highly recommended to set them in batch run.
        double timeoutInSecond;
        int randSeed;
        Iteration maxIter;      // max iteration to terminate.
        int threadNum;          // max number of threads used at the same time.
        std::string cfgPath;    // 
        std::string logPath;    // log file for record().
        std::string id;         // identify the job. very useful for multi-stage problems.
    }; // Environment
    #pragma endregion Type

    #pragma region Constant
public:
    #pragma endregion Constant

    #pragma region Constructor
public:
    Solver(const Problem &p, const Environment &e) : problem(p), opt(p), sln(p), solveStatus(e.timeoutInSecond) {}
    #pragma endregion Constructor

    #pragma region Method
public:
    // search for optimal solutions and record the best found one into opt.
    virtual void solve() { solvePlainModel(); }
    // check feasibility and recalculate objective function.
    virtual const Solver::CheckStatus& check() const; // may change checkStatus.
    // return a brief description of the solving algorithm.
    virtual std::string getConfig() const { return std::string("NA"); }
    // display a brief report for last search.
    void print() const;
    // write the report for last search into a log file.
    void record(const std::string &logPath) const;

    void solvePlainModel();
    void solveInconsistentVarModel();

protected:
    // must be called in the front of solve*().
    virtual void init(const std::string objDescription) {
        checkStatus.checked = false;
        opt.obj.value = Objective::MaxValue;
        opt.obj.description = objDescription;
        sln.obj.value = 0;
        sln.obj.description = objDescription;
    }

    bool updateOptima(const Solution<bool> &solution) {
        if (solution.obj.value < opt.obj.value) {
            opt.obj.value = solution.obj.value;
            opt.vars = solution.vars;
        }
        // EXTEND[szx][4]: handle equal case.
    }
    #pragma endregion Method

    #pragma region Field
public:
protected:
    Problem problem;
    Solution<bool> opt;
    Solution<bool> sln;
    
    Environment env;
    mutable CheckStatus checkStatus;
    SolveStatus solveStatus;
    #pragma endregion Field
}; // Solver

}


#endif // SZX_BOOLEAN_SATISFIABILITY_PROBLEM_SOLVER_H
