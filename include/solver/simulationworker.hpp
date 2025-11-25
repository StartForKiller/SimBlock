#pragma once

#include <qschematic/netlist.hpp>

#include <QMap>

class Operation;
class OperationConnector;

namespace Solver {

class SolverBase;

class SimulationWorker : public QObject {
    Q_OBJECT

    public:
        explicit SimulationWorker(QObject *parent = nullptr);
        ~SimulationWorker() override = default;

        void setNetlist(QSchematic::Netlist<Operation *, OperationConnector *> *netlist);
        void setTimeParameters(double timestep, double timeToSimulate);
        void simulate();

    signals:
        void sampleGenerated(double t, QMap<QString, double> values);
        void simulationFinished();

    private:
        SolverBase *_solver;
        QSchematic::Netlist<Operation *, OperationConnector *> *_netlist;
        double _timeToSimulate;
        double _timeStep;
};

}