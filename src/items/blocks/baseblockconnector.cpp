#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/popup/popup_baseblockconnector.hpp>

#include <qschematic/items/label.hpp>
#include <qschematic/scene.hpp>
#include <qschematic/commands/item_remove.hpp>
#include <qschematic/commands/item_visibility.hpp>
#include <qschematic/commands/label_rename.hpp>

#include <QPainter>
#include <QGraphicsSceneContextMenuEvent>
#include <QMenu>
#include <QInputDialog>

using namespace Blocks;

#define SIZE (_settings.gridSize / 4)
#define RECT (QRectF(-SIZE, -SIZE, 2 * SIZE, 2 * SIZE))

const QColor COLOR_BODY_FILL = QColor(Qt::black);
const QColor COLOR_BODY_BORDER = QColor(Qt::black);
const qreal PEN_WIDTH = 1.5;

BaseBlockConnector::BaseBlockConnector(const QPoint& gridPoint, const QString& text, bool input, uint index, QGraphicsItem *parent) :
    _isInput(input),
    _index(index),
    QSchematic::Items::Connector(::ItemType::BaseBlockConnectorType, gridPoint, text, parent)
{
    label()->setVisible(true);
    setForceTextDirection(false);
}

gpds::container BaseBlockConnector::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("connector", QSchematic::Items::Connector::to_container());
    root.add_value("isInput", _isInput);
    root.add_value<int>("index", _index);
    root.add_value<int>("mirror_orientation", (int)_mirrorOrientation);

    return root;
}

void BaseBlockConnector::from_container(const gpds::container &container) {
    QSchematic::Items::Connector::from_container(*container.get_value<gpds::container *>("connector").value());
    _isInput = container.get_value<bool>("isInput").value();
    _index = container.get_value<int>("index").value_or(0);
    _mirrorOrientation = (uint)container.get_value<int>("mirror_orientation").value_or(0);
    _mirrored = _mirrorOrientation != 0;
}

std::shared_ptr<QSchematic::Items::Item> BaseBlockConnector::deepCopy() const {
    auto clone = std::make_shared<BaseBlockConnector>(gridPos(), text(), parentItem());
    copyAttributes(*clone);

    return clone;
}

std::unique_ptr<QWidget> BaseBlockConnector::popup() const {
    return std::make_unique<PopupBaseBlockConnector>(*this);
}

void BaseBlockConnector::update() {
    // The item class sets the origin to the center of the bounding box
    // but in this case we want to rotate around the center of the body
    setTransformOriginPoint(boundingRect().center());
    QGraphicsObject::update();
}

QRectF BaseBlockConnector::boundingRect() const {
    qreal adj = 1.5;

    return RECT.adjusted(-adj, -adj, adj, adj);
}

void BaseBlockConnector::mirrorHorizontal(qreal width) {
    if(!(_mirrorOrientation & (uint)Qt::Horizontal)) {
        _mirrorOrientation |= (uint)Qt::Horizontal;
    } else {
        _mirrorOrientation &= ~(uint)Qt::Horizontal;
    }
    _mirrored = _mirrorOrientation != 0;

    setX(width - pos().x());
}

void BaseBlockConnector::mirrorVertical(qreal height) {
    if(!(_mirrorOrientation & (uint)Qt::Vertical)) {
        _mirrorOrientation |= (uint)Qt::Vertical;
    } else {
        _mirrorOrientation &= ~(uint)Qt::Vertical;
    }
    _mirrored = _mirrorOrientation != 0;

    setY(height - pos().y());
}

void BaseBlockConnector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget)

    if(_settings.debug) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(Qt::red));
        painter->drawRect(boundingRect());
    }

    QPen bodyPen;
    bodyPen.setWidthF(PEN_WIDTH);
    bodyPen.setStyle(Qt::SolidLine);
    bodyPen.setColor(COLOR_BODY_BORDER);

    QBrush bodyBrush;
    bodyBrush.setStyle(Qt::SolidPattern);
    bodyBrush.setColor(COLOR_BODY_FILL);

    painter->setPen(bodyPen);
    painter->setBrush(bodyBrush);

    bool reverseArrow = ((_mirrorOrientation & (int)Qt::Horizontal) != 0) && _mirrored;

    if(hasConnection()) {
        if(_isInput) {
            QPointF triPoints[3];
            triPoints[0] = QPointF(reverseArrow ? SIZE : -SIZE, -SIZE);
            triPoints[1] = QPointF(0.0, 0.0);
            triPoints[2] = QPointF(reverseArrow ? SIZE : -SIZE, SIZE);
            painter->drawPolygon(triPoints, 3);
        }
    } else if(_isInput) {
        QPointF triPoints[3];
        triPoints[0] = QPointF(reverseArrow ? SIZE : -SIZE, -SIZE);
        triPoints[1] = QPointF(0.0, 0.0);
        triPoints[2] = QPointF(reverseArrow ? SIZE : -SIZE, SIZE);
        painter->drawPolyline(triPoints, 3);
    } else {
        QPointF triPoints[3];
        triPoints[0] = QPointF(0.0, -SIZE);
        triPoints[1] = QPointF(reverseArrow ? -SIZE : SIZE, 0.0);
        triPoints[2] = QPointF(0.0, SIZE);
        painter->drawPolyline(triPoints, 3);
    }
}

void BaseBlockConnector::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    {
        QAction *visibility = new QAction;
        visibility->setCheckable(true);
        visibility->setChecked(isVisible());
        visibility->setText("Visible");
        connect(visibility, &QAction::toggled, [this](const bool enabled) {
            if(scene())
                scene()->undoStack()->push(new QSchematic::Commands::ItemVisibility(this->shared_from_this(), enabled));
            else
                setVisible(enabled);
        });

        QAction *labelVisibility = new QAction;
        labelVisibility->setCheckable(true);
        labelVisibility->setChecked(label()->isVisible());
        labelVisibility->setText("Label visible");
        connect(labelVisibility, &QAction::toggled, [this](bool enabled) {
            if(scene())
                scene()->undoStack()->push(new QSchematic::Commands::ItemVisibility(label(), enabled));
            else
                label()->setVisible(enabled);
        });

        QAction *text = new QAction;
        text->setText("Rename ...");
        connect(text, &QAction::triggered, [this]{
            bool ok = false;
            const QString &newText = QInputDialog::getText(
                nullptr,
                "Rename Connector",
                "New connector text",
                QLineEdit::Normal,
                label()->text(),
                &ok
            );
            if(!ok)
                return;

            if(scene())
                scene()->undoStack()->push(new QSchematic::Commands::LabelRename(label().get(), newText));
            else
                label()->setText(newText);
        });

        QAction *alignLabel = new QAction;
        alignLabel->setText("Align label");
        connect(alignLabel, &QAction::triggered, [this] {
            this->alignLabel();
        });

        QAction *deleteFromModel = new QAction;
        deleteFromModel->setText("Delete");
        connect(deleteFromModel, &QAction::triggered, [this] {
            if(scene()) {
                std::shared_ptr<QSchematic::Items::Item> itemPointer;
                {
                    const BaseBlock *operation = qgraphicsitem_cast<const BaseBlock *>(parentItem());
                    if(!operation)
                        return;

                    for(auto &i : operation->connectors()) {
                        if(i.get() == this) {
                            itemPointer = i;
                            break;
                        }
                    }
                    if(!itemPointer)
                        return;
                }

                scene()->undoStack()->push(new QSchematic::Commands::ItemRemove(scene(), itemPointer));
            }
        });

        menu.addAction(visibility);
        menu.addAction(labelVisibility);
        menu.addAction(text);
        menu.addAction(alignLabel);
        menu.addAction(deleteFromModel);
    }

    menu.exec(event->screenPos());
}

void BaseBlockConnector::copyAttributes(BaseBlockConnector &dest) const {
    QSchematic::Items::Connector::copyAttributes(dest);
    dest._isInput = _isInput;
    dest._index = _index;
    dest._mirrored = _mirrored;
    dest._mirrorOrientation = _mirrorOrientation;
}
