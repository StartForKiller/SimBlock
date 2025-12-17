#include <solver/simulationworker.hpp>
#include <solver/ode4solver.hpp>
#include <solver/ode45solver.hpp>

//#define SIMBLOCK_DEBUG_SOLVER 1

using namespace Solver;
using namespace Blocks;

SimulationWorker::SimulationWorker(QObject *parent) :
    QObject(parent)
{
    _solver = new Solver::ODE45Solver();
}

void SimulationWorker::setNetlist(QSchematic::Netlist<BaseBlock *, BaseBlockConnector *> *netlist) {
    _netlist = netlist;
}

void SimulationWorker::setTimeParameters(double timestep, double timeToSimulate) {
    _timeStep = timestep;
    _timeToSimulate = timeToSimulate;
}

void SimulationWorker::simulate() {
    if(!_netlist) {
        emit simulationFinished();
        return;
    }

    _solver->setup(*_netlist);
    auto initialOutputValues = _solver->getOutputValues();
    emit sampleGenerated(0.0, initialOutputValues);

    #ifdef SIMBLOCK_DEBUG_SOLVER
        for(auto [netName, netValue] : initialOutputValues.asKeyValueRange()) {
            printf("Solver (t=%f) Net(%s)=%lf\n",
                0.0,
                netName.toStdString().c_str(),
                std::get<double>(netValue.data));
        }
    #endif

    ((Solver::ODE45Solver *)_solver)->setMaxTimestep(_timeToSimulate);

    for(double t = 0; t < _timeToSimulate;) {
        double dt = _solver->solve_step(_timeStep);
        t += dt;

        ((Solver::ODE45Solver *)_solver)->setMaxTimestep(_timeToSimulate - t);

        auto netOutputValues = _solver->getOutputValues();
        emit sampleGenerated(t, netOutputValues);

        #ifdef SIMBLOCK_DEBUG_SOLVER
            for(auto [netName, netValue] : netOutputValues.asKeyValueRange()) {
                printf("Solver (t=%f) Net(%s)=%lf\n",
                    t,
                    netName.toStdString().c_str(),
                    std::get<double>(netValue.data));
            }
        #endif
    }

    emit simulationFinished();
}
