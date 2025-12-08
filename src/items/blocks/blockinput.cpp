#include <items/blocks/blockinput.hpp>
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

BlockInput::BlockInput(Windows::BaseWindow *window, QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockInputType, window, parent)
{
    setSize(40, 40);
    setBaseName(QStringLiteral("Input"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true, 0, QPoint(0, 1), QStringLiteral("in") },
        { false, 0, QPoint(2, 1), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);
    getConnector(true, 0)->setVisible(false);

    setDescription(QStringLiteral("Outputs the input of the subsystem's input"));
    addProperty({
        "Index", Properties::BLOCK_PROPERTY_INT,
        0, 1e9,
        [&](const QVariant &v) { _index = v.toDouble(); },
        [&]() { return QVariant(_index); }
    });

    //setConnectorsMovable(false);
}

gpds::container BlockInput::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());
    root.add_value("index", _index);

    return root;
}

void BlockInput::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
    _index = container.get_value<double>("index").value();
}

std::shared_ptr<QSchematic::Items::Item> BlockInput::deepCopy() const {
    auto clone = std::make_shared<BlockInput>(parentWindow(), parentItem());
    copyAttributes(*clone);

    return clone;
}

void BlockInput::copyAttributes(BlockInput &dest) const {
    BaseBlock::copyAttributes(dest);

    dest._index = _index;
}

Solver::BlockType BlockInput::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("input"),
        1, //Inputs
        1, //Outputs
        0, //States

        false //Only true for blocks that doesn't cause algebraic loops
    };
}

void BlockInput::solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) {
    out[0] = in[0];
}
