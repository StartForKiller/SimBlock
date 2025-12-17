#include <solver/solver.hpp>

namespace Solver {

class ODE4Solver : public SolverBase {
    public:
        explicit ODE4Solver();
        virtual ~ODE4Solver();

        double solve_step(double timestep) override;
};

}