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

double ODE45Solver::estimateError(const QVector<Signal>& y4, const QVector<Signal>& y5) {
    double maxErr = 0.0;

    int n = y4.size();
    for(int i = 0; i < n; i++) {
        double e = abs(std::get<double>((y5[i] - y4[i]).data)); //TODO
        maxErr = std::max(maxErr, e);
    }

    return maxErr;
}

bool ODE45Solver::tryStep(double h, double tolerance) {
    QVector<Signal> yt, y4, y5, k1, k2, k3, k4, k5, k6;

    int n = _y.size();
    yt.resize(n);
    y4.resize(n);
    y5.resize(n);

    f_global(_y, k1);

    for(int i = 0; i < n; i++)
        yt[i] = _y[i] + h * RK45Coeff::a21 * k1[i];
    f_global(yt, k2);

    for(int i = 0; i < n; i++)
        yt[i] = _y[i] + h * (RK45Coeff::a31 * k1[i] + RK45Coeff::a32 * k2[i]);
    f_global(yt, k3);

    for(int i = 0; i < n; i++)
        yt[i] = _y[i] + h * (RK45Coeff::a41 * k1[i] + RK45Coeff::a42 * k2[i] + RK45Coeff::a43 * k3[i]);
    f_global(yt, k4);

    for(int i = 0; i < n; i++)
        yt[i] = _y[i] + h * (RK45Coeff::a51 * k1[i] + RK45Coeff::a52 * k2[i]
                                                + RK45Coeff::a53 * k3[i] + RK45Coeff::a54 * k4[i]);
    f_global(yt, k5);

    for(int i = 0; i < n; i++)
        yt[i] = _y[i] + h * (RK45Coeff::a61 * k1[i] + RK45Coeff::a62 * k2[i]
                                                + RK45Coeff::a63 * k3[i] + RK45Coeff::a64 * k4[i]
                                                + RK45Coeff::a65 * k5[i]);
    f_global(yt, k6);

    for(int i = 0; i < n; i++) {
        y5[i] = _y[i] + h * (RK45Coeff::b1 * k1[i] + RK45Coeff::b3 * k3[i]
                                                + RK45Coeff::b4 * k4[i] + RK45Coeff::b5 * k5[i]
                                                + RK45Coeff::b6 * k6[i]);

        y4[i] = _y[i] + h * (RK45Coeff::b1s * k1[i] + RK45Coeff::b3s * k3[i]
                                                + RK45Coeff::b4s * k4[i] + RK45Coeff::b5s * k5[i]
                                                + RK45Coeff::b6s * k6[i]);
    }

    _err = estimateError(y4, y5);

    if(_err <= tolerance) {
        _y = y5;
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
