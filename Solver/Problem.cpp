#include <iostream>
#include <fstream>

#include "Problem.h"


using namespace std;


namespace szx {

istream& operator>>(istream &is, Problem &problem) {
    char c;
    int v;
    string s;

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

void Problem::load(const string &path) {
    ifstream ifs(path);
    ifs >> *this;
}

}
