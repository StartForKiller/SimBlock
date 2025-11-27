#include <items/blocks/blocksum.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>

#include <qschematic/items/label.hpp>

using namespace Blocks;

BlockSum::BlockSum(QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockSumType, parent)
{
    setSize(80, 80);
    setBaseName(QStringLiteral("Sum"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true , 0, QPoint(0, 1), QStringLiteral("in 1") },
        { true , 1, QPoint(0, 3), QStringLiteral("in 2") },
        { false, 0, QPoint(4, 2), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Outputs the sum of both inputs"));
}

gpds::container BlockSum::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());

    return root;
}

void BlockSum::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
}

std::shared_ptr<QSchematic::Items::Item> BlockSum::deepCopy() const {
    auto clone = std::make_shared<BlockSum>(parentItem());
    copyAttributes(*clone);

    return clone;
}

Solver::BlockType BlockSum::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("sum"),
        2, //Inputs
        1, //Outputs
        0  //States
    };
}

void BlockSum::solveAlgebraic(const QVector<double> &in, QVector<double> &out, const QVector<double> &params, const QVector<double> &states) {
    out[0] = in[0] + in[1];
}

void BlockSum::copyAttributes(BlockSum &dest) const {
    BaseBlock::copyAttributes(dest);
}
