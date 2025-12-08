#include <items/blocks/blockconstant.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>

#include <qschematic/scene.hpp>
#include <qschematic/items/label.hpp>
#include <qschematic/commands/item_remove.hpp>
#include <qschematic/commands/item_visibility.hpp>
#include <qschematic/commands/label_rename.hpp>

#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>

using namespace Blocks;

BlockConstant::BlockConstant(Windows::BaseWindow *window, QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockConstantType, window, parent)
{
    setSize(40, 40);
    setBaseName(QStringLiteral("Constant"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { false, 0, QPoint(2, 1), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Outputs a constant value"));
    addProperty({
        "Value", Properties::BLOCK_PROPERTY_DOUBLE,
        -1e9, 1e9,
        [&](const QVariant &v) { _constantValue = v.toDouble(); },
        [&]() { return QVariant(_constantValue); }
    });

    //setConnectorsMovable(false);
}

gpds::container BlockConstant::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());
    root.add_value("value", _constantValue);

    return root;
}

void BlockConstant::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
    _constantValue = container.get_value<double>("value").value();
}

std::shared_ptr<QSchematic::Items::Item> BlockConstant::deepCopy() const {
    auto clone = std::make_shared<BlockConstant>(parentWindow(), parentItem());
    copyAttributes(*clone);

    return clone;
}

void BlockConstant::copyAttributes(BlockConstant &dest) const {
    BaseBlock::copyAttributes(dest);

    dest._constantValue = _constantValue;
}

Solver::BlockType BlockConstant::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("constant"),
        0, //Inputs
        1, //Outputs
        0, //States

        false //Only true for blocks that doesn't cause algebraic loops
    };
}

void BlockConstant::solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) {
    out[0] = Solver::make_signal(_constantValue);
}
