#include <solver/solver.hpp>

namespace Solver {

class ODE4Solver : public SolverBase {
    public:
        explicit ODE4Solver();
        virtual ~ODE4Solver();

        void setup(const QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> &netlist) override;

        double solve_step(double timestep) override;

    private:
        QVector<Signal> _xtmp, _k1, _k2, _k3, _k4;
};

}