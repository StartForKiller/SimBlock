#pragma once

#include <qschematic/netlist.hpp>


#include <string>
#include <QVector>
#include <QMap>
#include <functional>

namespace Blocks {
    class BaseBlock;
    class BaseBlockConnector;
};

using SignalID = int;
namespace Solver {

struct Block {
    Blocks::BaseBlock *node;
    QString name;
    QString type;
    QVector<SignalID> inputs;
    QVector<SignalID> outputs;
    QVector<double> params;
    QVector<double> states;
};

struct BlockType {
    QString name;
    int numInputs;
    int numOutputs;
    int numStates;
};

class SolverBase {
    public:
        explicit SolverBase();
        virtual ~SolverBase();

        void setup(const QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> &netlist);
        void solve_step(double timestep);

        QMap<QString, double> getOutputValues();

    private:
        void evaluateAlgebraic();
        void f_global(const QMap<QString, QVector<double>> &y, QMap<QString, QVector<double>> &xdot);
        void ode4_step(QMap<QString, QVector<double>> &y, double dt);

        QMap<QString, SignalID> _signalMap;
        QVector<double> _signals;
        QMap<QString, BlockType> _blockTypes;
        QVector<Block> _blocks;
        QMap<QString, QVector<double>> _y;
};

}