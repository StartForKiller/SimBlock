#include <solver/solver.hpp>
#include <items/operation.hpp>
#include <items/operationconnector.hpp>
#include <items/operationscope.hpp>

using namespace Solver;

SolverBase::SolverBase()
{

}

SolverBase::~SolverBase() {

}

void SolverBase::setup(const QSchematic::Netlist<Operation *, OperationConnector *> &netlist) {
    //Populate blocks and block types
    {
        _blocks.clear();
        for(auto &node : netlist.nodes) {
            auto blocktype = node->getSolverBlockType();
            _blocks.append({
                node->text(),
                blocktype.name,
                QMap<QString, SignalID>(),
                QMap<QString, SignalID>(),
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
            //blk.inputs.resize(_blockTypes[blk.type].numInputs);
            //blk.outputs.resize(_blockTypes[blk.type].numOutputs);
            blk.states.resize(_blockTypes[blk.type].numStates);
        }

        for(const auto &net : netlist.nets) {
            SignalID s = _signalMap[net.name];

            for(auto &[conn, node] : net.connectorNodePairs) {
                for(auto &blk : _blocks) {
                    if(blk.name != node->text()) continue;

                    if(conn->label()->text().toStdString().find("out") != std::string::npos) {
                        blk.outputs[conn->label()->text()] = s;
                    } else if(conn->label()->text().toStdString().find("in") != std::string::npos) {
                        blk.inputs[conn->label()->text()] = s;

                        auto scopeNode = dynamic_cast<OperationScope *>(node);
                        if(scopeNode != nullptr) {
                            scopeNode->setInputNetName(net.name);
                        }
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

        QMap<QString, double> in;
        QMap<QString, double> out;

        for(auto [name, input] : blk.inputs.asKeyValueRange()) {
            in[name] = _signals[input];
        }

        if(type.algebraic != nullptr) type.algebraic(in, out, blk.params, blk.states);

        for(auto [name, output] : blk.outputs.asKeyValueRange()) {
            _signals[output] = out[name];
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
        if(type.numStates == 0 || type.derivative == nullptr) continue;

        QMap<QString, double> in;
        QVector<double> dx(type.numStates);

        for(auto [name, input] : blk.inputs.asKeyValueRange()) {
            in[name] = _signals[input];
        }

        type.derivative(in, blk.states, dx, blk.params);

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
