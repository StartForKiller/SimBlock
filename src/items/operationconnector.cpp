#include <items/operationconnector.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/popup/popup_connector.hpp>

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

#define SIZE (_settings.gridSize / 2)
#define RECT (QRectF(-SIZE, -SIZE, 2 * SIZE, 2 * SIZE))

const QColor COLOR_BODY_FILL = QColor(Qt::white);
const QColor COLOR_BODY_BORDER = QColor(Qt::black);
const qreal PEN_WIDTH = 1.5;

OperationConnector::OperationConnector(const QPoint& gridPoint, const QString& text, QGraphicsItem *parent) :
    QSchematic::Items::Connector(::ItemType::OperationConnectorType, gridPoint, text, parent)
{
    label()->setVisible(true);
    setForceTextDirection(false);
}

gpds::container OperationConnector::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("connector", QSchematic::Items::Connector::to_container());

    return root;
}

void OperationConnector::from_container(const gpds::container &container) {
    QSchematic::Items::Connector::from_container(*container.get_value<gpds::container *>("connector").value());
}

std::shared_ptr<QSchematic::Items::Item> OperationConnector::deepCopy() const {
    auto clone = std::make_shared<OperationConnector>(gridPos(), text(), parentItem());
    copyAttributes(*clone);

    return clone;
}

std::unique_ptr<QWidget> OperationConnector::popup() const {
    return std::make_unique<PopupConnector>(*this);
}

QRectF OperationConnector::boundingRect() const {
    qreal adj = 1.5;

    return RECT.adjusted(-adj, -adj, adj, adj);
}

void OperationConnector::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
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
    painter->drawEllipse(RECT);
}

void OperationConnector::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
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

void OperationConnector::copyAttributes(OperationConnector &dest) const {
    QSchematic::Items::Connector::copyAttributes(dest);
}
