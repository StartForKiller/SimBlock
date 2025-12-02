#include <solver/testsolver.hpp>
#include <items/blocks/baseblock.hpp>

using namespace Solver;

TestSolver::TestSolver(Blocks::BaseBlock *block) :
    _block(block)
{
    reset();
}

void TestSolver::setInput(QVector<Signal> in) {
    _in = in;
}

void TestSolver::setInput(Signal in) {
    _in.clear();
    _in.append(in);
}

Signal TestSolver::output() const {
    return _out[0];
}

QVector<Signal> TestSolver::outputVec() const {
    return _out;
}

QVector<Signal> TestSolver::step(double dt) {
    int numStates = _block->getSolverBlockType().numStates;
    for(int i = 0; i < numStates; i++) {
        _states[i] = _y[i];
    }

    QVector<Signal> dx(numStates);
    _block->solveDerivative(_in, _states, dx);

    for(int i = 0; i < numStates; i++) {
        _y[i] += dt * dx[i];
    }

    for(int i = 0; i < numStates; i++) {
        _states[i] = _y[i];
    }
    _block->solveAlgebraic(_in, _out, _states);

    return _out;
}

void TestSolver::reset() {
    _in.clear(); _out.clear(); _states.clear(); _y.clear();

    _in.resize(_block->getSolverBlockType().numInputs, make_signal(0));
    _out.resize(_block->getSolverBlockType().numOutputs, make_signal(0));
    _states.resize(_block->getSolverBlockType().numStates, make_signal(0));
    _y.resize(_block->getSolverBlockType().numStates, make_signal(0));
}