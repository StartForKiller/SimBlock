#include <items/operationdemo1.hpp>
#include <items/operationconnector.hpp>
#include <items/itemtypes.hpp>

#include <qschematic/items/label.hpp>

struct ConnectorAttribute {
    QPoint point;
    QString name;
};

OperationDemo1::OperationDemo1(QGraphicsItem *parent) :
    BaseBlock(::ItemType::OperationDemo1Type, parent)
{
    setSize(160, 160);
    label()->setText(QStringLiteral("Demo 1"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { QPoint(0, 2), QStringLiteral("in 1") },
        { QPoint(0, 4), QStringLiteral("in 2") },
        { QPoint(0, 6), QStringLiteral("in 3") },
        { QPoint(8, 4), QStringLiteral("out") }
    };

    for(const auto &c : connectorAttributes) {
        auto connector = std::make_shared<OperationConnector>(c.point, c.name);
        connector->label()->setVisible(true);
        addConnector(connector);
    }
}

gpds::container OperationDemo1::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());

    return root;
}

void OperationDemo1::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
}

std::shared_ptr<QSchematic::Items::Item> OperationDemo1::deepCopy() const {
    auto clone = std::make_shared<OperationDemo1>(parentItem());
    copyAttributes(*clone);

    return clone;
}

Solver::BlockType OperationDemo1::getSolverBlockType() {
    using namespace std::placeholders;
    return {
        QStringLiteral("Demo1"),
        3,
        1,
        1,

        std::bind(&OperationDemo1::solveAlgebraic, this, _1, _2, _3, _4),
        std::bind(&OperationDemo1::solveDerivative, this, _1, _2, _3, _4)
    };
}

void OperationDemo1::solveAlgebraic(const QMap<QString, double> &in, QMap<QString, double> &out, const QVector<double> &params, const QVector<double> &states) {
    out[QStringLiteral("out")] = states[0];
}

void OperationDemo1::solveDerivative(const QMap<QString, double> &in, const QVector<double> &states, QVector<double> &xdot, const QVector<double> &params) {
    xdot[0] = in[QStringLiteral("in 1")];
}

void OperationDemo1::copyAttributes(OperationDemo1 &dest) const {
    BaseBlock::copyAttributes(dest);
}
