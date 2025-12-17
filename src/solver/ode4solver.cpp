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
    QVector<Signal> xtmp, k1, k2, k3, k4;

    int n = _y.size();
    xtmp.resize(n);

    f_global(_y, k1);

    for(int i = 0; i < n; i++)
        xtmp[i] = _y[i] + 0.5 * dt * k1[i];
    f_global(xtmp, k2);

    for(int i = 0; i < n; i++)
        xtmp[i] = _y[i] + 0.5 * dt * k2[i];
    f_global(xtmp, k3);

    for(int i = 0; i < n; i++)
        xtmp[i] = _y[i] + dt * k3[i];
    f_global(xtmp, k4);

    for(int i = 0; i < n; i++)
        _y[i] += (dt/6.0) * (k1[i] + 2.0*k2[i] + 2.0*k3[i] + k4[i]);

    evaluateAlgebraic();

    return dt;
}
