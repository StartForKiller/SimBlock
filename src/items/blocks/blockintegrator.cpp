#include <items/blocks/blockintegrator.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>

#include <qschematic/items/label.hpp>

using namespace Blocks;

BlockIntegrator::BlockIntegrator(QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockIntegratorType, parent)
{
    setSize(80, 80);
    setBaseName(QStringLiteral("Integrator"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true , 0, QPoint(0, 2), QStringLiteral("in") },
        { false, 0, QPoint(4, 2), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Integrates the input value"));
}

gpds::container BlockIntegrator::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());

    return root;
}

void BlockIntegrator::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
}

std::shared_ptr<QSchematic::Items::Item> BlockIntegrator::deepCopy() const {
    auto clone = std::make_shared<BlockIntegrator>(parentItem());
    copyAttributes(*clone);

    return clone;
}

Solver::BlockType BlockIntegrator::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("integrator"),
        1, //Inputs
        1, //Outputs
        1  //States
    };
}

void BlockIntegrator::solveAlgebraic(const QVector<double> &in, QVector<double> &out, const QVector<double> &params, const QVector<double> &states) {
    out[0] = states[0];
}

void BlockIntegrator::solveDerivative(const QVector<double> &in, const QVector<double> &states, QVector<double> &xdot, const QVector<double> &params) {
    xdot[0] = in[0];
}

void BlockIntegrator::copyAttributes(BlockIntegrator &dest) const {
    BaseBlock::copyAttributes(dest);
}
