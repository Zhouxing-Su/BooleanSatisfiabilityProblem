#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <mutex>

#include <cmath>

#include "Solver.h"
#include "MpSolver.h"


using namespace std;


namespace szx {

#pragma region Solver
#pragma region Solver::CmdInterface
int Solver::CmdInterface::run(int argc, char *argv[]) {
    Environment env;
    env.instancePath = "C168_FW_UT_518.cnf";

    Problem problem;
    problem.load(env.instancePath);

    Solver solver(problem, env);
    solver.solve();
    cout << solver.check() << endl;
    cout << solver.sln << endl;

    solver.solveInconsistentVarModel();
    cout << solver.check() << endl;
    cout << solver.sln << endl;

    return 0;
}
#pragma endregion Solver::CmdInterface

const Solver::CheckStatus& Solver::check() const {
    if (checkStatus.checked) { return checkStatus; }
    checkStatus.checked = true;

    checkStatus.feasible = true;
    checkStatus.objMatch = true;
    for (auto c = problem.clauses.begin(); c != problem.clauses.end(); ++c) {
        int sum = 0;
        for (auto l = c->begin(); l != c->end(); ++l) {
            int varIndex = *l;
            sum += ((varIndex > 0) ? sln.vars.at(varIndex) : (1 - sln.vars.at(-varIndex)));
        }
        if (sum == 0) {
            checkStatus.feasible = false;
            // EXTEND[szx][9]: report violation.
        }
    }

    // EXTEND[szx][6]: check objective.

    return checkStatus;
}

void Solver::record(const string &logPath) const {
    check();
    ostringstream log;

    double duration = Timer::getDuration(solveStatus.timer.startTime, opt.obj.findTime);
    log << Timer::getLocalTime() << ","
        << env.id << ","
        << env.instancePath << ","
        << getConfig() << ","
        << env.randSeed << ","
        << solveStatus.generation << "," << solveStatus.iteration << ","
        << ((duration >= 0) ? to_string(duration) : "NA") << ","
        << checkStatus.feasible << ","
        << (checkStatus.objMatch ? 0.0 : (checkStatus.objVal - opt.obj.value)) << ","
        << opt.obj.value << ",";

    // TODO[szx][0]: record solution vector

    log << endl;

    static mutex logFileMutex;
    lock_guard<mutex> logFileGuard(logFileMutex);

    // append all text. this should be atomic since ios::ate does not seek to end on each write.
    ofstream logFile(logPath, ios::app);
    logFile.seekp(0, ios::end);
    if (logFile.tellp() <= 0) {
        logFile << "Time,ID,Instance,Config,RandSeed,Generation,Iteration,Duration,Feasible,Check-Obj,ObjValue,Solution" << endl;
    }
    logFile << log.str();
    logFile.close();
}

void Solver::solvePlainModel() {
    init("MinUnsatClauseNum");

    MpSolver mp;

    // add decision variables.
    Arr<MpSolver::DecisionVar> dvarX(problem.varIndexEnd);
    for (auto x = dvarX.begin() + Problem::VarIndexBegin; x != dvarX.end(); ++x) {
        *x = mp.addDecisionVar(0, 1, 0, MpSolver::NumberType::Bool);
    }

    mp.updateModel();

    // set objective (no objective).
    mp.setOptimaOrientation(MpSolver::OptimaOrientation::Maximize);

    // add constraints.
    for (auto c = problem.clauses.begin(); c != problem.clauses.end(); ++c) {
        MpSolver::LinearExpr expr;
        for (auto l = c->begin(); l < c->end(); ++l) {
            int varIndex = *l;
            expr += ((varIndex > 0) ? dvarX.at(varIndex) : (1 - dvarX.at(-varIndex)));
        }
        mp.addConstraint(expr >= 1);
    }

    // solve model.
    if (!mp.solve()) {
        mp.relax(false, true, false, MpSolver::RelaxObjType::Cardinality);
        mp.solve();
        sln.obj.value = static_cast<Objective::Value>(mp.getObjectiveValue());
    }
    //mp.saveModel("model0.lp");

    // record decision.
    if (MpSolver::solutionFound(mp.getStatus())) {
        for (int x = Problem::VarIndexBegin; x < problem.varIndexEnd; ++x) {
            sln.vars.at(x) = MpSolver::isTrue(dvarX.at(x));
        }
    }
}

void Solver::solveInconsistentVarModel() {
    init("MinInconsistentVarNum");

    MpSolver mp;

    // add decision variables.
    Arr<vector<MpSolver::DecisionVar>> dvarX(problem.varIndexEnd);
    Arr<vector<MpSolver::DecisionVar>> dvarY(problem.varIndexEnd);
    for (auto c = problem.clauses.begin(); c != problem.clauses.end(); ++c) {
        for (auto l = c->begin(); l < c->end(); ++l) {
            dvarX.at(abs(*l)).push_back(mp.addDecisionVar(0, 1, 0, MpSolver::NumberType::Bool));
            dvarY.at(abs(*l)).push_back(mp.addDecisionVar(0, 1, 1, MpSolver::NumberType::Real));
        }
    }

    mp.updateModel();

    // set objective (already set when adding decision variables).
    mp.setOptimaOrientation(MpSolver::OptimaOrientation::Minimize);

    // add constraints.
    Arr<int> varCount(dvarX.size(), -1);
    for (auto c = problem.clauses.begin(); c != problem.clauses.end(); ++c) {
        MpSolver::LinearExpr expr;
        for (auto l = c->begin(); l < c->end(); ++l) {
            int varIndex = *l;
            expr += ((varIndex > 0) ? dvarX.at(varIndex).at(++varCount.at(varIndex))
                : (1 - dvarX.at(-varIndex).at(++varCount.at(-varIndex))));
        }
        mp.addConstraint(expr >= 1);
    }

    for (int x = Problem::VarIndexBegin; x < dvarX.size(); ++x) {
        const vector<MpSolver::DecisionVar> &dx(dvarX.at(x));
        const vector<MpSolver::DecisionVar> &dy(dvarY.at(x));
        int literalNum = static_cast<int>(dx.size());
        if (literalNum <= 1) { continue; } // no need to worry about consistency if there is less than one occurrence.
        for (int i = 0; i < literalNum; ++i) {
            int i1 = (i + 1) % literalNum;
            mp.addConstraint(dy.at(i) >= (dx.at(i) - dx.at(i1)));
            mp.addConstraint(dy.at(i) >= (dx.at(i1) - dx.at(i)));
        }
    }

    // solve model.
    mp.solve();
    //mp.saveModel("model1.lp");

    // record decision.
    if (MpSolver::solutionFound(mp.getStatus())) {
        sln.obj.value = static_cast<Objective::Value>(mp.getObjectiveValue());
        for (int x = Problem::VarIndexBegin; x < problem.varIndexEnd; ++x) {
            if (dvarX.at(x).empty()) { continue; }
            sln.vars.at(x) = MpSolver::isTrue(dvarX.at(x).front());
        }
    }
}
#pragma endregion Solver

}

