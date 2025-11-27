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

    //setConnectorsMovable(false);

    setupConnectors(connectorAttributes);
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

void BlockGain::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
    QMenu menu;
    {
        QAction *text = new QAction;
        text->setText("Rename ...");
        connect(text, &QAction::triggered, [this]{
            if(!scene())
                return;

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

            if(nameIsInUse(newText)) return;

            scene()->undoStack()->push(new QSchematic::Commands::LabelRename(label().get(), newText));
        });

        QAction *isMovable = new QAction;
        isMovable->setCheckable(true);
        isMovable->setChecked(this->isMovable());
        isMovable->setText("Is Movable");
        connect(isMovable, &QAction::toggled, [this](bool enabled) {
            setMovable(enabled);
        });

        QAction *labelVisibility = new QAction;
        labelVisibility->setCheckable(true);
        labelVisibility->setChecked(label()->isVisible());
        labelVisibility->setText("Label visible");
        connect(labelVisibility, &QAction::toggled, [this](bool enabled) {
            if(!scene())
                return;

            scene()->undoStack()->push(new QSchematic::Commands::ItemVisibility(label(), enabled));
        });

        QAction *alignLabel = new QAction;
        alignLabel->setText("Align label");
        connect(alignLabel, &QAction::triggered, [this] {
            this->alignLabel();
        });

        QAction *value = new QAction;
        value->setText("Gain Value ...");
        connect(value, &QAction::triggered, [this]{
            if(!scene())
                return;

            bool ok = false;
            const double newDouble = QInputDialog::getDouble(
                nullptr,
                "Set Constant Value",
                "New constant value",
                _gainValue,
                -2147483647, 2147483647, 5,
                &ok
            );
            if(!ok)
                return;

            //scene()->undoStack()->push(new QSchematic::Commands::LabelRename(label().get(), newText)); //TODO
            _gainValue = newDouble;
        });

        QAction *duplicate = new QAction;
        duplicate->setText("Duplicate");
        connect(duplicate, &QAction::triggered, [this]{
            if(!scene())
                return;

            auto clone = deepCopy();
            clone->setPos(pos() + QPointF(5 * _settings.gridSize, 5 * _settings.gridSize));
            scene()->addItem(std::move(clone));
        });

        QAction *deleteFromModel = new QAction;
        deleteFromModel->setText("Delete");
        connect(deleteFromModel, &QAction::triggered, [this] {
            if(!scene())
                return;

            std::shared_ptr<QSchematic::Items::Item> itemPointer;
            for(auto &i : scene()->items()) {
                if(i.get() == this) {
                    itemPointer = i;
                    break;
                }
            }
            if(!itemPointer)
                return;

            scene()->undoStack()->push(new QSchematic::Commands::ItemRemove(scene(), itemPointer));
        });

        menu.addAction(text);
        menu.addAction(labelVisibility);
        menu.addAction(alignLabel);
        menu.addSeparator();
        menu.addAction(value);
        menu.addSeparator();
        menu.addAction(duplicate);
        menu.addAction(deleteFromModel);
        menu.addSeparator();
        menu.addAction(isMovable);
    }

    menu.exec(event->screenPos());
}
