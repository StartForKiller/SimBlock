#include <items/blocks/blockgain.hpp>
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

BlockGain::BlockGain(QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockGainType, parent)
{
    setSize(40, 40);
    label()->setText(QStringLiteral("Gain"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true, 0, QPoint(0, 1), QStringLiteral("in") },
        { false, 0, QPoint(2, 1), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);

    addProperty({
        "Gain", Properties::BLOCK_PROPERTY_DOUBLE,
        -1e9, 1e9,
        [&](const QVariant &v) { _gainValue = v.toDouble(); },
        [&]() { return QVariant(_gainValue); }
    });

    //setConnectorsMovable(false);
}

gpds::container BlockGain::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());
    root.add_value("gain", _gainValue);

    return root;
}

void BlockGain::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
    _gainValue = container.get_value<double>("gain").value();
}

std::shared_ptr<QSchematic::Items::Item> BlockGain::deepCopy() const {
    auto clone = std::make_shared<BlockGain>(parentItem());
    copyAttributes(*clone);

    return clone;
}

void BlockGain::copyAttributes(BlockGain &dest) const {
    BaseBlock::copyAttributes(dest);

    dest._gainValue = _gainValue;
}

Solver::BlockType BlockGain::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("gain"),
        1, //Inputs
        1, //Outputs
        0  //States
    };
}

void BlockGain::solveAlgebraic(const QVector<double> &in, QVector<double> &out, const QVector<double> &params, const QVector<double> &states) {
    out[0] = in[0] * _gainValue;
}
