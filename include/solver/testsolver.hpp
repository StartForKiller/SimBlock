#pragma once

#include <solver/solver.hpp>

namespace Blocks {
    class BaseBlock;
};

namespace Solver {

class TestSolver {
    public:
        explicit TestSolver(Blocks::BaseBlock *block);

        void setInput(Signal in);
        void setInput(QVector<Signal> in);
        Signal output() const;
        QVector<Signal> outputVec() const;

        QVector<Signal> step(double dt);
        void reset();

    private:
        Blocks::BaseBlock *_block;
        QVector<Signal> _states;
        QVector<Signal> _y;
        QVector<Signal> _in;
        QVector<Signal> _out;
};

}