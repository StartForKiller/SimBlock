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
#include <QPainter>

using namespace Blocks;

BlockGain::BlockGain(QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockGainType, parent)
{
    setSize(40, 40);
    setBaseName(QStringLiteral("Gain"));

    QVector<ConnectorAttribute> connectorAttributes = {
        { true, 0, QPoint(0, 1), QStringLiteral("in") },
        { false, 0, QPoint(2, 1), QStringLiteral("out") }
    };
    setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Applies a gain to the input"));
    addProperty({
        "Gain", Properties::BLOCK_PROPERTY_DOUBLE,
        -1e9, 1e9,
        [&](const QVariant &v) { _gainValue = v.toDouble(); update(); },
        [&]() { return QVariant(_gainValue); }
    });

    setConnectorsMovable(false);
    setConnectorsSnapToGrid(false);
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

void BlockGain::sizeChangedEvent(QSizeF oldSize, QSizeF newSize) {
    BaseBlock::sizeChangedEvent(oldSize, newSize);

    auto c = getConnector(true, 0);
    if(c) c->setPos(QPointF(0, newSize.height() / 2.0));
    c = getConnector(false, 0);
    if(c) c->setPos(QPointF(newSize.width(), newSize.height() / 2.0));
}

const QColor COLOR_BODY_FILL = QColor(QStringLiteral("#E0E0E0"));
const QColor COLOR_BODY_BORDER = QColor(Qt::black);
const qreal TEXT_MARGIN_LEFT = 5.0;
const qreal PEN_WIDTH = 1.5;

void BlockGain::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget)

    if(_settings.debug) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(Qt::red));
        painter->drawRect(boundingRect());
    }

    //Body
    {
        QPointF triPoints[3];

        QPointF a(0.0, 0.0), b(0.0, height()), c(width(), height()/2.0);
        QPolygonF triangle;
        triangle << a << b << c;

        QPen pen;
        pen.setWidthF(PEN_WIDTH);
        pen.setStyle(Qt::SolidLine);
        pen.setColor(COLOR_BODY_BORDER);

        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(COLOR_BODY_FILL);

        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawPolygon(triangle);

        auto text = QStringLiteral("%1").arg(_gainValue);
        auto fallback = QStringLiteral("K");

        QFont font = painter->font();
        font.setPointSize(11);
        painter->setFont(font);

        QFontMetrics fm(font);
        qreal textW = fm.horizontalAdvance(text);
        qreal textH = fm.height();
        QSizeF textSize(textW, textH);

        QRectF triBounds = triangle.boundingRect();
        QPointF topLeft(triBounds.left() + TEXT_MARGIN_LEFT, triBounds.center().y() - textH / 2.0);
        QRectF textRect(topLeft, textSize);

        auto rectInsidePolygon = [&](const QRectF &r, const QPolygonF &poly) -> bool {
            QPointF p1 = r.topLeft();
            QPointF p2 = r.topRight();
            QPointF p3 = r.bottomRight();
            QPointF p4 = r.bottomLeft();
            // usa OddEvenFill (normal) para containsPoint
            return poly.containsPoint(p1, Qt::OddEvenFill)
                && poly.containsPoint(p2, Qt::OddEvenFill)
                && poly.containsPoint(p3, Qt::OddEvenFill)
                && poly.containsPoint(p4, Qt::OddEvenFill);
        };

        QString toDraw = text;
        QRectF drawRect = textRect;

        if (!rectInsidePolygon(textRect, triangle)) {
            toDraw = fallback;
            qreal fW = fm.horizontalAdvance(toDraw);
            qreal fH = fm.height();
            QPointF fTopLeft(triBounds.left() + TEXT_MARGIN_LEFT, triBounds.center().y() - fH / 2.0);
            drawRect = QRectF(fTopLeft, QSizeF(fW, fH));
        }

        painter->drawText(drawRect, Qt::AlignCenter, toDraw);
    }

    if(isSelected() && allowMouseResize()) {
        paintResizeHandles(*painter);
    }

    if(isSelected() && allowMouseRotate()) {
        paintRotateHandle(*painter);
    }
}

Solver::BlockType BlockGain::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("gain"),
        1, //Inputs
        1, //Outputs
        0, //States

        false //Only true for blocks that doesn't cause algebraic loops
    };
}

void BlockGain::solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) {
    out[0] = in[0] * _gainValue;
}
