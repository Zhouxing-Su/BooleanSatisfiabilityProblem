#include <string>

#include "Solver.h"
#include "MpSolver.h"


using namespace std;


namespace szx {

int Solver::CmdInterface::run(int argc, char *argv[]) {
    string instancePath("C168_FW_UT_518.cnf");

    Problem problem;
    problem.load(instancePath);

    Solver solver(problem);
    solver.solve();

    cout << solver.sln;

    return 0;
}

void Solver::solvePlainModel() {
    MpSolver mp;

    // set objective.
    mp.setOptimaOrientation(MpSolver::OptimaOrientation::Maximize);

    // add decision variables.
    Arr<MpSolver::DecisionVar> dvars(problem.varIndexEnd);
    for (auto x = dvars.begin() + Problem::VarIndexBegin; x != dvars.end(); ++x) {
        *x = mp.addDecisionVar(0, 1, 0, MpSolver::NumberType::Bool);
    }

    mp.updateModel();

    // add constraints.
    for (auto c = problem.clauses.begin(); c != problem.clauses.end(); ++c) {
        MpSolver::LinearExpr expr;
        for (auto l = c->begin(); l < c->end(); ++l) {
            expr += ((*l > 0) ? dvars.at(*l) : (1 - dvars.at(-*l)));
        }
        mp.addConstraint(expr >= 1);
    }

    // solve model.
    if (!mp.solve()) {
        mp.relax(false, true, false, MpSolver::RelaxObjType::Cardinality);
        mp.solve();
        sln.unsatClauseNum = static_cast<Solution::Objective>(mp.getObjectiveValue());
    }
    //mp.saveModel("model.lp");

    // record decision.
    if (MpSolver::solutionFound(mp.getStatus())) {
        for (int x = Problem::VarIndexBegin; x < problem.varIndexEnd; ++x) {
            sln.vars.at(x) = MpSolver::isTrue(dvars.at(x));
        }
    }
}

}

