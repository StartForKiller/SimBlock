#include <solver/ode45solver.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/blockscope.hpp>
#include <items/blocks/blocksubsystem.hpp>

#include <QQueue>

using namespace Solver;
using namespace Blocks;

ODE45Solver::ODE45Solver()
{

}

ODE45Solver::~ODE45Solver() {

}

void ODE45Solver::setup(const QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> &netlist) {
    SolverBase::setup(netlist);

    int n = _y.size();

    _yt.resize(n);
    _y4.resize(n);
    _y5.resize(n);
    _k1.resize(n);
    _k2.resize(n);
    _k3.resize(n);
    _k4.resize(n);
    _k5.resize(n);
    _k6.resize(n);
}

double ODE45Solver::estimateError(const QVector<Signal>& _y4, const QVector<Signal>& _y5) {
    double maxErr = 0.0;

    int n = _y4.size();
    for(int i = 0; i < n; i++) {
        double e = abs(std::get<double>((_y5[i] - _y4[i]).data)); //TODO
        maxErr = std::max(maxErr, e);
    }

    return maxErr;
}

bool ODE45Solver::tryStep(double h, double tolerance) {
    int n = _y.size();

    f_global(_y, _k1);

    for(int i = 0; i < n; i++)
        _yt[i] = _y[i] + h * RK45Coeff::a21 * _k1[i];
    f_global(_yt, _k2);

    for(int i = 0; i < n; i++)
        _yt[i] = _y[i] + h * (RK45Coeff::a31 * _k1[i] + RK45Coeff::a32 * _k2[i]);
    f_global(_yt, _k3);

    for(int i = 0; i < n; i++)
        _yt[i] = _y[i] + h * (RK45Coeff::a41 * _k1[i] + RK45Coeff::a42 * _k2[i] + RK45Coeff::a43 * _k3[i]);
    f_global(_yt, _k4);

    for(int i = 0; i < n; i++)
        _yt[i] = _y[i] + h * (RK45Coeff::a51 * _k1[i] + RK45Coeff::a52 * _k2[i]
                                                + RK45Coeff::a53 * _k3[i] + RK45Coeff::a54 * _k4[i]);
    f_global(_yt, _k5);

    for(int i = 0; i < n; i++)
        _yt[i] = _y[i] + h * (RK45Coeff::a61 * _k1[i] + RK45Coeff::a62 * _k2[i]
                                                + RK45Coeff::a63 * _k3[i] + RK45Coeff::a64 * _k4[i]
                                                + RK45Coeff::a65 * _k5[i]);
    f_global(_yt, _k6);

    for(int i = 0; i < n; i++) {
        _y5[i] = _y[i] + h * (RK45Coeff::b1 * _k1[i] + RK45Coeff::b3 * _k3[i]
                                                + RK45Coeff::b4 * _k4[i] + RK45Coeff::b5 * _k5[i]
                                                + RK45Coeff::b6 * _k6[i]);

        _y4[i] = _y[i] + h * (RK45Coeff::b1s * _k1[i] + RK45Coeff::b3s * _k3[i]
                                                + RK45Coeff::b4s * _k4[i] + RK45Coeff::b5s * _k5[i]
                                                + RK45Coeff::b6s * _k6[i]);
    }

    _err = estimateError(_y4, _y5);

    if(_err <= tolerance) {
        _y = _y5;
        evaluateAlgebraic();
        return true;
    }

    return false;
}

double ODE45Solver::adaptStep(double h, double tolerance) {
    constexpr double safety = 0.9;
    constexpr double minFactor = 0.2;
    constexpr double maxFactor = 5.0;
    constexpr double exponent = 0.2; // 1/5

    // Protección: error cero → podemos crecer
    if(_err <= std::numeric_limits<double>::epsilon()) {
        return h * maxFactor;
    }

    double factor = safety * std::pow(tolerance / _err, exponent);

    factor = std::clamp(factor, minFactor, maxFactor);

    return h * factor;
}

double ODE45Solver::solve_step(double tolerance) {
    double h = _h;
    double dt_eff = 0.0;

    while(dt_eff == 0.0) {
        if(_maxTimeStep > 0) h = std::min(_maxTimeStep, h);
        if(tryStep(h, tolerance)) {
            dt_eff = h;
            _h = adaptStep(h, tolerance);
        } else {
            h *= 0.5;
        }
    }

    return dt_eff;
}
