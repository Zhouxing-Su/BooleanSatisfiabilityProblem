#include <iostream>
#include <fstream>

#include "Solution.h"


using namespace std;


namespace szx {

std::ostream& operator<<(std::ostream &os, const Solution &sln) {
    os << "MinUnsat " << sln.unsatClauseNum << endl;
    for (int x = Problem::VarIndexBegin; x < sln.vars.size(); ++x) {
        os << " " << (sln.vars.at(x) ? x : -x);
    }

    return os;
}

void Solution::save(const std::string &path) const {
    ofstream ofs(path);
    ofs << *this;
}

}
