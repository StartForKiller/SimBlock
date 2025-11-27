#include <items/blocks/blockscope.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>
#include <scope/scopewindow.hpp>

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

BlockScope::BlockScope(QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockScopeType, parent)
{
    setSize(80, 80);
    setBaseName(QStringLiteral("Scope"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true, 0, QPoint(0, 2), QStringLiteral("in") }
    };
    setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Displays values on the input on the GUI"));

    //setConnectorsMovable(false);
}

gpds::container BlockScope::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());

    return root;
}

void BlockScope::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
}

std::shared_ptr<QSchematic::Items::Item> BlockScope::deepCopy() const {
    auto clone = std::make_shared<BlockScope>(parentItem());
    copyAttributes(*clone);

    return clone;
}

void BlockScope::copyAttributes(BlockScope &dest) const {
    BaseBlock::copyAttributes(dest);
}

Solver::BlockType BlockScope::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("scope"),
        1, //Inputs
        0, //Outputs
        0  //States
    };
}

void BlockScope::setInputNetName(QString name) {
    _netName = name;
}

void BlockScope::generateScopeWindow() {
    if(_scopeWindow == nullptr) {
        _scopeWindow = new Scope::ScopeWindow(text());
    }
}

void BlockScope::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    generateScopeWindow();

    _scopeWindow->raise();
    _scopeWindow->activateWindow();
    _scopeWindow->show();


    BaseBlock::Node::mouseDoubleClickEvent(event);
}

void BlockScope::onNewSample(double t, QMap<QString, Solver::Signal> values) {
    generateScopeWindow();

    if(values.contains(_netName)) {
        _scopeWindow->onNewSample(t, values[_netName]);
    }
}

void BlockScope::onStartSimulation() {
    generateScopeWindow();

    _netName = QStringLiteral("");

    _scopeWindow->onStartSimulation();
}
