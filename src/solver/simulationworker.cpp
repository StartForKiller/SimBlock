#include <solver/simulationworker.hpp>
#include <solver/solver.hpp>

#define SIMBLOCK_DEBUG_SOLVER 1

using namespace Solver;
using namespace Blocks;

SimulationWorker::SimulationWorker(QObject *parent) :
    QObject(parent)
{
    _solver = new Solver::SolverBase();
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
    for(double t = 0; t <= _timeToSimulate; t += _timeStep) {
        _solver->solve_step(_timeStep);

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
