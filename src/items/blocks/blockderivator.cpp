#include <items/blocks/blockderivator.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>

#include <qschematic/items/label.hpp>

using namespace Blocks;

BlockDerivator::BlockDerivator(Windows::BaseWindow *window, QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockDerivatorType, window, parent)
{
    setSize(80, 80);
    setBaseName(QStringLiteral("Derivator"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true , 0, QPoint(0, 2), QStringLiteral("in") },
        { false, 0, QPoint(4, 2), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Derivates the input value"));
    addProperty({
        "Approx Frecuency", Properties::BLOCK_PROPERTY_DOUBLE,
        0, 1e9,
        [&](const QVariant &v) { _N = v.toDouble(); },
        [&]() { return QVariant(_N); }
    });
}

gpds::container BlockDerivator::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());
    root.add_value("value", _N);

    return root;
}

void BlockDerivator::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
    _N = container.get_value<double>("value").value_or(_N);
}

std::shared_ptr<QSchematic::Items::Item> BlockDerivator::deepCopy() const {
    auto clone = std::make_shared<BlockDerivator>(parentWindow(), parentItem());
    copyAttributes(*clone);

    return clone;
}

Solver::BlockType BlockDerivator::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("derivator"),
        1, //Inputs
        1, //Outputs
        1, //States

        false //Only true for blocks that doesn't cause algebraic loops
    };
}

void BlockDerivator::solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) {
    out[0] = -_N*_N * states[0] + _N * in[0];
}

void BlockDerivator::solveDerivative(const QVector<Solver::Signal> &in, const QVector<Solver::Signal> &states, QVector<Solver::Signal> &xdot) {
    xdot[0] = -_N * states[0] + in[0];
}

void BlockDerivator::copyAttributes(BlockDerivator &dest) const {
    BaseBlock::copyAttributes(dest);
}
