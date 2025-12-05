#include <items/blocks/blocksubsystem.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>
#include <windows/subsystemwindow.hpp>

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

BlockSubsystem::BlockSubsystem(QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockSubsystemType, parent)
{
    setSize(80, 80);
    setBaseName(QStringLiteral("Subsystem"));

    //QVector<ConnectorAttribute> connectorAttributes = {
    //    { true, 0, QPoint(0, 2), QStringLiteral("in") }
    //};
    //setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Subsystem description TODO"));

    //setConnectorsMovable(false);
}

gpds::container BlockSubsystem::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());
    if(_subsystemWindow != nullptr) root.add_value("scene", _subsystemWindow->to_container());
    root.add_value("inputs", _inputs);
    root.add_value("outputs", _outputs);

    return root;
}

void BlockSubsystem::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
    auto tempScene = container.get_value<gpds::container *>("scene").value_or(nullptr);
    if(tempScene != nullptr) {
        _subsystemSceneContainer = *tempScene;
        _subsystemSceneContainerValid = true;
    }
    _inputs = container.get_value<int>("inputs").value_or(0);
    _outputs = container.get_value<int>("outputs").value_or(0);
}

std::shared_ptr<QSchematic::Items::Item> BlockSubsystem::deepCopy() const {
    auto clone = std::make_shared<BlockSubsystem>(parentItem());
    copyAttributes(*clone);

    return clone;
}

void BlockSubsystem::copyAttributes(BlockSubsystem &dest) const {
    BaseBlock::copyAttributes(dest);
}

Solver::BlockType BlockSubsystem::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("subsystem"),
        _inputs, //Inputs
        _outputs, //Outputs
        0, //States

        false //Only true for blocks that doesn't cause algebraic loops
    };
}

void BlockSubsystem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    generateSubsystemWindow();

    _subsystemWindow->raise();
    _subsystemWindow->activateWindow();
    _subsystemWindow->show();

    BaseBlock::Node::mouseDoubleClickEvent(event);
}

void BlockSubsystem::generateSubsystemWindow() {
    if(_subsystemWindow == nullptr) {
        _subsystemWindow = new Windows::SubsystemWindow();

        if(_subsystemSceneContainerValid) _subsystemWindow->from_container(_subsystemSceneContainer);
    }
}
