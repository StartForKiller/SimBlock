#include <solver/ode4solver.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/blockscope.hpp>
#include <items/blocks/blocksubsystem.hpp>

#include <QQueue>

using namespace Solver;
using namespace Blocks;

ODE4Solver::ODE4Solver()
{

}

ODE4Solver::~ODE4Solver() {

}

double ODE4Solver::solve_step(double dt) {
    QMap<QString, QVector<Signal>> xtmp, k1, k2, k3, k4;

    for(auto &blk : _blocks) {
        int n = _y[blk.name].size();
        xtmp[blk.name].resize(n);
    }

    f_global(_y, k1);

    for(auto &blk : _blocks) {
        int n = _y[blk.name].size();
        for(int i = 0; i < n; i++)
            xtmp[blk.name][i] = _y[blk.name][i] + 0.5 * dt * k1[blk.name][i];
    }
    f_global(xtmp, k2);

    for(auto &blk : _blocks) {
        int n = _y[blk.name].size();
        for(int i = 0; i < n; i++)
            xtmp[blk.name][i] = _y[blk.name][i] + 0.5 * dt * k2[blk.name][i];
    }
    f_global(xtmp, k3);

    for(auto &blk : _blocks) {
        int n = _y[blk.name].size();
        for(int i = 0; i < n; i++)
            xtmp[blk.name][i] = _y[blk.name][i] + dt * k3[blk.name][i];
    }
    f_global(xtmp, k4);

    for(auto &blk : _blocks) {
        int n = _y[blk.name].size();
        for(int i = 0; i < n; i++)
            _y[blk.name][i] += (dt/6.0) * (k1[blk.name][i] + 2.0*k2[blk.name][i] + 2.0*k3[blk.name][i] + k4[blk.name][i]);
    }

    evaluateAlgebraic();

    return dt;
}
