#include <items/operationscope.hpp>
#include <items/operationconnector.hpp>
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

struct ConnectorAttribute {
    QPoint point;
    QString name;
};

OperationScope::OperationScope(QGraphicsItem *parent) :
    Operation(::ItemType::OperationScopeType, parent)
{
    setSize(80, 80);
    label()->setText(QStringLiteral("Scope"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { QPoint(0, 2), QStringLiteral("in") }
    };

    //setConnectorsMovable(false);

    for(const auto &c : connectorAttributes) {
        auto connector = std::make_shared<OperationConnector>(c.point, c.name);
        connector->label()->setVisible(false);
        addConnector(connector);
    }
}

gpds::container OperationScope::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", Operation::to_container());

    return root;
}

void OperationScope::from_container(const gpds::container &container) {
    Operation::from_container(*container.get_value<gpds::container *>("operation").value());
}

std::shared_ptr<QSchematic::Items::Item> OperationScope::deepCopy() const {
    auto clone = std::make_shared<OperationScope>(parentItem());
    copyAttributes(*clone);

    return clone;
}

void OperationScope::copyAttributes(OperationScope &dest) const {
    Operation::copyAttributes(dest);
}

Solver::BlockType OperationScope::getSolverBlockType() {
    using namespace std::placeholders;
    return {
        QStringLiteral("scope"),
        1, //Inputs
        0, //Outputs
        0, //States

        nullptr,
        nullptr
    };
}

void OperationScope::setInputNetName(QString name) {
    printf("Scope NET name: %s\n", name.toStdString().c_str());

    _netName = name;
}

void OperationScope::generateScopeWindow() {
    if(_scopeWindow == nullptr) {
        _scopeWindow = new Scope::ScopeWindow(QStringLiteral("Scope"));
    }
}

void OperationScope::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    generateScopeWindow();

    _scopeWindow->raise();
    _scopeWindow->activateWindow();
    _scopeWindow->show();


    Operation::Node::mouseDoubleClickEvent(event);
}

void OperationScope::onNewSample(double t, QMap<QString, double> values) {
    generateScopeWindow();

    if(values.contains(_netName)) {
        printf("Value %f\n", values[_netName]);
        _scopeWindow->onNewSample(t, values[_netName]);
    }
}

void OperationScope::onStartSimulation() {
    generateScopeWindow();

    _scopeWindow->onStartSimulation();
}
