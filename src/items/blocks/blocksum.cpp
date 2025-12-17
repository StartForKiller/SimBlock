#include <items/blocks/blocksum.hpp>
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
#include <QPainter>
#include <QRegularExpression>

using namespace Blocks;

BlockSum::BlockSum(Windows::BaseWindow *window, QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockSumType, window, parent)
{
    setSize(40, 40);
    setBaseName(QStringLiteral("Sum"));

    updateConnectors();

    setDescription(QStringLiteral("Outputs the sum of both inputs"));
    addProperty({
        "Gain", Properties::BLOCK_PROPERTY_STRING,
        -1e9, 1e9,
        [&](const QVariant &v) {
            auto stringValue = v.toString();
            if(stringValue.size() > 3 || stringValue.isEmpty()) return;

            int countValid = stringValue.count(QRegularExpression("[+\\-|]"));
            if(countValid != 3) return;

            _descriptorString = stringValue;
            updateConnectors();
        },
        [&]() { return QVariant(_descriptorString); }
    });

    setConnectorsMovable(false);
    setConnectorsSnapToGrid(false);
}

gpds::container BlockSum::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());
    root.add_value("descriptor", _descriptorString.toStdString());

    return root;
}

void BlockSum::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
    _descriptorString = QString::fromStdString(container.get_value<std::string>("descriptor").value_or("|++"));
    updateConnectors();
}

std::shared_ptr<QSchematic::Items::Item> BlockSum::deepCopy() const {
    auto clone = std::make_shared<BlockSum>(parentWindow(), parentItem());
    copyAttributes(*clone);

    return clone;
}

void BlockSum::sizeChangedEvent(QSizeF oldSize, QSizeF newSize) {
    BaseBlock::sizeChangedEvent(oldSize, newSize);
}

const QColor COLOR_BODY_FILL = QColor(QStringLiteral("#E0E0E0"));
const QColor COLOR_BODY_BORDER = QColor(Qt::black);
const qreal TEXT_MARGIN_LEFT = 5.0;
const qreal PEN_WIDTH = 1.5;

void BlockSum::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget)

    if(_settings.debug) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(Qt::red));
        painter->drawRect(boundingRect());
    }

    //Body
    {
        QPen pen;
        pen.setWidthF(PEN_WIDTH);
        pen.setStyle(Qt::SolidLine);
        pen.setColor(COLOR_BODY_BORDER);

        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(COLOR_BODY_FILL);

        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawEllipse(QRectF(0.0, 0.0, width(), height()));

        auto width = size().width();
        auto height = size().height();

        QFont font = painter->font();
        font.setPointSize(11);
        painter->setFont(font);

        qreal adj = 2;

        for(int i = 0; i < 3; i++) {
            auto charValue = _descriptorString.toStdString().c_str()[i];
            if(charValue == '|') continue;

            QFontMetrics fm(font);
            qreal textW = fm.horizontalAdvance(charValue);
            qreal textH = fm.height();
            switch(i) {
                case 0: //Top
                    painter->drawText(QPointF((width / 2) - (textW / 2), textW + adj), QString(charValue));
                    break;
                case 1: //Left
                    painter->drawText(QPointF(adj, (height / 2) + (textW / 2) - adj), QString(charValue));
                    break;
                case 2: //Bottom
                    painter->drawText(QPointF((width / 2) - (textW / 2), height - adj), QString(charValue));
                    break;
            }
        }
    }

    if(isSelected() && allowMouseResize()) {
        paintResizeHandles(*painter);
    }
}

Solver::BlockType BlockSum::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("sum"),
        _inputs, //Inputs
        1, //Outputs
        0, //States

        false //Only true for blocks that doesn't cause algebraic loops
    };
}

void BlockSum::solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) {
    switch(_inputs) {
        case 3:
            out[0] = _gains[0] * in[0] + _gains[1] * in[1] + _gains[2] * in[2]; break;
        case 2:
            out[0] = _gains[0] * in[0] + _gains[1] * in[1]; break;
        default:
            out[0] = _gains[0] * in[0]; break;
    }
}

void BlockSum::copyAttributes(BlockSum &dest) const {
    BaseBlock::copyAttributes(dest);
}

void BlockSum::updateConnectors() {
    //TODO
    auto width_grid = size().width() / settings().gridSize;
    auto height_grid = size().height() / settings().gridSize;

    QVector<ConnectorAttribute> connectorAttributes = {
        { false, 0, QPoint(width_grid, height_grid / 2), QStringLiteral("out") }
    };
    int index = 0;
    for(int i = 0; i < 3; i++) {
        auto charValue = _descriptorString.toStdString().c_str()[i];
        if(charValue == '|') continue;
        switch(i) {
            case 0: //Top
                connectorAttributes.append({ true, index, QPoint(width_grid / 2, 0), QStringLiteral("in %1").arg(index) });
                break;
            case 1: //Left
                connectorAttributes.append({ true, index, QPoint(0, height_grid / 2), QStringLiteral("in %1").arg(index) });
                break;
            case 2: //Bottom
                connectorAttributes.append({ true, index, QPoint(width_grid / 2, height_grid), QStringLiteral("in %1").arg(index) });
                break;
        }
        if(charValue == '-') _gains[index] = -1;
        else _gains[index] = 1;
        index++;
    }
    _inputs = index;
    setupConnectors(connectorAttributes);

    index = 0;
    for(int i = 0; i < 3; i++) {
        auto charValue = _descriptorString.toStdString().c_str()[i];
        if(charValue == '|') continue;
        switch(i) {
            case 0: //Top
                getConnector(true, index)->setRotation(90);
                break;
            case 2: //Bottom
                getConnector(true, index)->setRotation(-90);
                break;
        }
        index++;
    }
}
