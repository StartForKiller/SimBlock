#include <solver/solver.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/blockscope.hpp>

using namespace Solver;
using namespace Blocks;

SolverBase::SolverBase()
{

}

SolverBase::~SolverBase() {

}

void SolverBase::setup(const QSchematic::Netlist<BaseBlock *, BaseBlockConnector *> &netlist) {
    //Populate blocks and block types
    {
        _blocks.clear();
        for(auto &node : netlist.nodes) {
            auto blocktype = node->getSolverBlockType();
            _blocks.append({
                node,
                node->text(),
                blocktype.name,
                QVector<SignalID>(blocktype.numInputs),
                QVector<SignalID>(blocktype.numOutputs),
                node->getSolverParams()
            });

            if(!_blockTypes.contains(blocktype.name))
                _blockTypes[blocktype.name] = blocktype;
        }
    }

    //Assign Signals
    {
        _signals.clear();
        _signalMap.clear();

        SignalID nextID = 0;
        for(const auto &net : netlist.nets) {
            _signalMap[net.name] = nextID++;
        }
        _signals.resize(nextID, 0.0);
    }

    //Wire Blocks
    {
        for(auto &blk : _blocks) {
            blk.inputs.clear();
            blk.outputs.clear();
            blk.inputs.resize(_blockTypes[blk.type].numInputs, -1); //TODO
            blk.outputs.resize(_blockTypes[blk.type].numOutputs, -1); //TODO
            blk.states.resize(_blockTypes[blk.type].numStates);
        }

        for(const auto &net : netlist.nets) {
            SignalID s = _signalMap[net.name];

            for(auto &[conn, node] : net.connectorNodePairs) {
                for(auto &blk : _blocks) {
                    if(blk.name != node->text()) continue;

                    if(conn->isInput()) {
                        blk.inputs[conn->index()] = s;

                        auto scopeNode = dynamic_cast<BlockScope *>(node);
                        if(scopeNode != nullptr) {
                            scopeNode->setInputNetName(net.name);
                        }

                    } else {
                        blk.outputs[conn->index()] = s;
                    }
                }
            }
        }
    }

    _y.clear();
    for(auto &blk : _blocks) {
        _y[blk.name].clear();

        auto &type = _blockTypes[blk.type];
        _y[blk.name].resize(type.numStates, 0.0);
    }
}

QMap<QString, double> SolverBase::getOutputValues() {
    QMap<QString, double> output;

    for(auto &blk : _blocks) {
        for(auto outSignal : blk.outputs) {
            auto netName = _signalMap.key(outSignal);
            output[netName] = _signals[outSignal];
        }
    }

    return output;
}

void SolverBase::solve_step(double timestep) {
    ode4_step(_y, timestep);
}

void SolverBase::evaluateAlgebraic() {
    for(auto &blk : _blocks) {
        auto &type = _blockTypes[blk.type];

        QVector<double> in(type.numInputs);
        QVector<double> out(type.numOutputs);

        for(int i = 0; i < type.numInputs; i++) {
            int inSignal = blk.inputs[i];
            if(inSignal >= 0) in[i] = _signals[inSignal];
        }

        blk.node->solveAlgebraic(in, out, blk.params, blk.states);

        for(int i = 0; i < type.numOutputs; i++) {
            int outSignal = blk.outputs[i];
            if(outSignal >= 0) _signals[outSignal] = out[i];
        }
    }
}

void SolverBase::f_global(const QMap<QString, QVector<double>> &y, QMap<QString, QVector<double>> &xdot) {
    for(auto &blk : const_cast<QVector<Block> &>(_blocks)) {
        int idx = 0;
        auto &type = _blockTypes[blk.type];
        for(int i = 0; i < type.numStates; i++) {
            const_cast<Block &>(blk).states[i] = y[blk.name][idx++];
        }
    }

    evaluateAlgebraic();

    xdot.clear();
    for(auto &blk : _blocks) {
        auto &type = _blockTypes[blk.type];
        if(type.numStates == 0) continue;

        QVector<double> in(type.numInputs);
        QVector<double> dx(type.numStates);

        for(int i = 0; i < type.numInputs; i++) {
            int inSignal = blk.inputs[i];
            if(inSignal >= 0) in[i] = _signals[inSignal];
        }

        blk.node->solveDerivative(in, blk.states, dx, blk.params);

        for(double d : dx) {
            int idx = 0;
            xdot[blk.name].push_back(d);
        }
    }
}

void SolverBase::ode4_step(QMap<QString, QVector<double>> &y, double dt) {
    QMap<QString, QVector<double>> xtmp, k1, k2, k3, k4;

    for(auto &blk : _blocks) {
        int n = y[blk.name].size();
        xtmp[blk.name].resize(n);
    }

    f_global(y, k1);

    for(auto &blk : _blocks) {
        int n = y[blk.name].size();
        for(int i = 0; i < n; i++)
            xtmp[blk.name][i] = y[blk.name][i] + 0.5 * dt * k1[blk.name][i];
    }
    f_global(xtmp, k2);

    for(auto &blk : _blocks) {
        int n = y[blk.name].size();
        for(int i = 0; i < n; i++)
            xtmp[blk.name][i] = y[blk.name][i] + 0.5 * dt * k2[blk.name][i];
    }
    f_global(xtmp, k3);

    for(auto &blk : _blocks) {
        int n = y[blk.name].size();
        for(int i = 0; i < n; i++)
            xtmp[blk.name][i] = y[blk.name][i] + dt * k3[blk.name][i];
    }
    f_global(xtmp, k4);

    for(auto &blk : _blocks) {
        int n = y[blk.name].size();
        for(int i = 0; i < n; i++)
            y[blk.name][i] += (dt/6.0) * (k1[blk.name][i] + 2.0*k2[blk.name][i] + 2.0*k3[blk.name][i] + k4[blk.name][i]);
    }
}
