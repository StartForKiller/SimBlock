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

void ODE4Solver::setup(const QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> &netlist) {
    SolverBase::setup(netlist);

    int n = _y.size();

    _xtmp.resize(n);
    _k1.resize(n);
    _k2.resize(n);
    _k3.resize(n);
    _k4.resize(n);
}

double ODE4Solver::solve_step(double dt) {
    int n = _y.size();

    f_global(_y, _k1);

    for(int i = 0; i < n; i++)
        _xtmp[i] = _y[i] + 0.5 * dt * _k1[i];
    f_global(_xtmp, _k2);

    for(int i = 0; i < n; i++)
        _xtmp[i] = _y[i] + 0.5 * dt * _k2[i];
    f_global(_xtmp, _k3);

    for(int i = 0; i < n; i++)
        _xtmp[i] = _y[i] + dt * _k3[i];
    f_global(_xtmp, _k4);

    for(int i = 0; i < n; i++)
        _y[i] += (dt/6.0) * (_k1[i] + 2.0*_k2[i] + 2.0*_k3[i] + _k4[i]);

    evaluateAlgebraic();

    return dt;
}
