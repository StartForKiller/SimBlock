#pragma once

#include <qschematic/netlist.hpp>


#include <string>
#include <QVector>
#include <QMap>
#include <functional>
#include <variant>

namespace Blocks {
    class BaseBlock;
    class BaseBlockConnector;
};

using SignalID = int;
namespace Solver {

struct Signal {
    using Scalar = double;
    using Vector = QVector<double>;
    using Bus = QMap<QString, double>;

    std::variant<Scalar, Vector, Bus> data;
};

struct Block {
    Blocks::BaseBlock *node;
    QString name;
    QString type;
    QVector<SignalID> inputs;
    QVector<SignalID> outputs;
    QVector<Signal> states;
    int stateOffset = -1;
};

struct BlockType {
    QString name;
    int numInputs;
    int numOutputs;
    int numStates;
    bool dontConsumeInput;
};

class SolverBase {
    public:
        explicit SolverBase();
        virtual ~SolverBase();

        void setup(const QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> &netlist);
        virtual double solve_step(double argument);

        QMap<QString, Signal> getOutputValues();

    private:
        QMap<QString, SignalID> _signalMap;
        QVector<Signal> _signals;
        QMap<QString, BlockType> _blockTypes;
        QVector<Block *> _orderedBlocks;

    protected:
        void evaluateAlgebraic();
        void f_global(const QVector<Signal> &y, QVector<Signal> &xdot);

        QVector<Block> _blocks;
        QVector<Signal> _y;
};

inline Signal make_signal(double x) {
    Signal s;
    s.data = x;
    return s;
}

inline bool isScalar(const Signal& s) {
    return std::holds_alternative<Signal::Scalar>(s.data);
}

inline bool isVector(const Signal& s) {
    return std::holds_alternative<Signal::Vector>(s.data);
}

inline bool isBus(const Signal& s) {
    return std::holds_alternative<Signal::Bus>(s.data);
}

Signal operator*(const Signal &a, const Signal &b);
inline Signal operator*(const Signal& s, double k) {
    return s * make_signal(k);
}

inline Signal operator*(double k, const Signal& s) {
    return make_signal(k) * s;
}

Signal operator/(const Signal &a, const Signal &b);
inline Signal operator/(const Signal& s, double k) {
    return s / make_signal(k);
}

inline Signal operator/(double k, const Signal& s) {
    return make_signal(k) / s;
}

Signal operator+(const Signal &a, const Signal &b);
inline Signal& operator+=(Signal& s, const Signal& other) {
    s = s + other;
    return s;
}

Signal operator-(const Signal &a, const Signal &b);

}