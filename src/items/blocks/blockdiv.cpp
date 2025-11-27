#include <items/blocks/blockdiv.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>

#include <qschematic/items/label.hpp>

using namespace Blocks;

BlockDiv::BlockDiv(QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockDivType, parent)
{
    setSize(80, 80);
    setBaseName(QStringLiteral("Div"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true , 0, QPoint(0, 1), QStringLiteral("in 1") },
        { true , 1, QPoint(0, 3), QStringLiteral("in 2") },
        { false, 0, QPoint(4, 2), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Divide a scalar value"));
}

gpds::container BlockDiv::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());

    return root;
}

void BlockDiv::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
}

std::shared_ptr<QSchematic::Items::Item> BlockDiv::deepCopy() const {
    auto clone = std::make_shared<BlockDiv>(parentItem());
    copyAttributes(*clone);

    return clone;
}

Solver::BlockType BlockDiv::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("div"),
        2, //Inputs
        1, //Outputs
        0  //States
    };
}

void BlockDiv::solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) {
    out[0] = in[0] / in[1];
}

void BlockDiv::copyAttributes(BlockDiv &dest) const {
    BaseBlock::copyAttributes(dest);
}
