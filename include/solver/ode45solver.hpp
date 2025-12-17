#include <solver/solver.hpp>

namespace Solver {

struct RK45Coeff {
    static constexpr double a21 = 1.0/5.0;

    static constexpr double a31 = 3.0/40.0;
    static constexpr double a32 = 9.0/40.0;

    static constexpr double a41 = 44.0/45.0;
    static constexpr double a42 = -56.0/15.0;
    static constexpr double a43 = 32.0/9.0;

    static constexpr double a51 = 19372.0/6561.0;
    static constexpr double a52 = -25360.0/2187.0;
    static constexpr double a53 = 64448.0/6561.0;
    static constexpr double a54 = -212.0/729.0;

    static constexpr double a61 = 9017.0/3168.0;
    static constexpr double a62 = -355.0/33.0;
    static constexpr double a63 = 46732.0/5247.0;
    static constexpr double a64 = 49.0/176.0;
    static constexpr double a65 = -5103.0/18656.0;

    static constexpr double c2 = 1.0/5.0;
    static constexpr double c3 = 3.0/10.0;
    static constexpr double c4 = 4.0/5.0;
    static constexpr double c5 = 8.0/9.0;

    static constexpr double b1 = 35.0/384.0;
    static constexpr double b3 = 500.0/1113.0;
    static constexpr double b4 = 125.0/192.0;
    static constexpr double b5 = -2187.0/6784.0;
    static constexpr double b6 = 11.0/84.0;

    static constexpr double b1s = 5179.0/57600.0;
    static constexpr double b3s = 7571.0/16695.0;
    static constexpr double b4s = 393.0/640.0;
    static constexpr double b5s = -92097.0/339200.0;
    static constexpr double b6s = 187.0/2100.0;
    static constexpr double b7s = 1.0/40.0;
};

class ODE45Solver : public SolverBase {
    public:
        explicit ODE45Solver();
        virtual ~ODE45Solver();

        double solve_step(double tolerance) override;

        void setMaxTimestep(double h) { _maxTimeStep = h; }

    private:
        bool tryStep(double h, double tolerance);
        double estimateError(const QVector<Signal>& y4, const QVector<Signal>& y5);
        double adaptStep(double h, double tolerance);

        double _maxTimeStep = -1;
        double _h = 1e-3;
        double _err = 0.0;
};

}