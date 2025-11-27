#pragma once

#include <qschematic/netlist.hpp>

#include <QMap>

namespace Blocks {
    class BaseBlock;
    class BaseBlockConnector;
};

namespace Solver {

class SolverBase;
struct Signal;

class SimulationWorker : public QObject {
    Q_OBJECT

    public:
        explicit SimulationWorker(QObject *parent = nullptr);
        ~SimulationWorker() override = default;

        void setNetlist(QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> *netlist);
        void setTimeParameters(double timestep, double timeToSimulate);
        void simulate();

    signals:
        void sampleGenerated(double t, QMap<QString, Signal> values);
        void simulationFinished();

    private:
        SolverBase *_solver;
        QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> *_netlist;
        double _timeToSimulate;
        double _timeStep;
};

}