#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/properties/blockpropertiesdialog.hpp>
#include <items/popup/popup_baseblock.hpp>
#include <commands/node_add_connector.hpp>

#include <windows/mainwindow.hpp>

#include <qschematic/scene.hpp>
#include <qschematic/items/label.hpp>
#include <qschematic/commands/item_remove.hpp>
#include <qschematic/commands/item_visibility.hpp>
#include <qschematic/commands/label_rename.hpp>

#include <QPainter>
#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>

using namespace Blocks;

const QColor COLOR_BODY_FILL = QColor(QStringLiteral("#E0E0E0"));
const QColor COLOR_BODY_BORDER = QColor(Qt::black);
const QColor SHADOW_COLOR = QColor(63, 63, 63, 100);
const qreal PEN_WIDTH = 1.5;
const qreal SHADOW_OFFSET = 7;
const qreal SHADOW_BLUR_RADIUS = 10;

BaseBlock::BaseBlock(int type, QGraphicsItem *parent) :
    QSchematic::Items::Node(type, parent)
{
    _label = std::make_shared<QSchematic::Items::Label>();
    _label->setParentItem(this);
    _label->setVisible(true);
    _label->setMovable(true);
    _label->setPos(0, 120);
    _label->setText(QStringLiteral("Generic"));
    connect(this, &QSchematic::Items::Node::sizeChanged, [this]{
        label()->setConnectionPoint(sizeRect().center());
        alignLabel();
    });
    connect(this, &QSchematic::Items::Node::settingsChanged, [this]{
        label()->setConnectionPoint(sizeRect().center());
        label()->setSettings(_settings);
    });

    setSize(160, 80);
    setAllowMouseResize(true);
    setAllowMouseRotate(true);
    setConnectorsMovable(true);
    setConnectorsSnapPolicy(QSchematic::Items::Connector::NodeSizerectOutline);
    setConnectorsSnapToGrid(true);

    auto graphicsEffect = new QGraphicsDropShadowEffect(this);
    graphicsEffect->setOffset(SHADOW_OFFSET);
    graphicsEffect->setBlurRadius(SHADOW_BLUR_RADIUS);
    graphicsEffect->setColor(SHADOW_COLOR);
    setGraphicsEffect(graphicsEffect);

    _description = QStringLiteral("Generic Description (This is an error)");
}

BaseBlock::~BaseBlock() {
    dissociate_item(_label);
}

gpds::container BaseBlock::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("node", QSchematic::Items::Node::to_container());
    root.add_value("label", _label->to_container());

    return root;
}

void BaseBlock::from_container(const gpds::container &container) {
    QSchematic::Items::Node::from_container(*container.get_value<gpds::container *>("node").value());
    _label->from_container(*container.get_value<gpds::container *>("label").value());
}

std::shared_ptr<QSchematic::Items::Item> BaseBlock::deepCopy() const {
    auto clone = std::make_shared<BaseBlock>(::ItemType::BaseBlockType, parentItem());
    copyAttributes(*clone);

    return clone;
}

std::unique_ptr<QWidget> BaseBlock::popup() const {
    return std::make_unique<PopupBaseBlock>(*this);
}

void BaseBlock::setBaseName(QString baseName) {
    _baseName = baseName;
    label()->setText(baseName);
}

void BaseBlock::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) {
    Q_UNUSED(option);
    Q_UNUSED(widget)

    if(_settings.debug) {
        painter->setPen(Qt::NoPen);
        painter->setBrush(QBrush(Qt::red));
        painter->drawRect(boundingRect());
    }

    //Body
    {
        qreal radius = _settings.gridSize / 2;

        QPen pen;
        pen.setWidthF(PEN_WIDTH);
        pen.setStyle(Qt::SolidLine);
        pen.setColor(COLOR_BODY_BORDER);

        QBrush brush;
        brush.setStyle(Qt::SolidPattern);
        brush.setColor(COLOR_BODY_FILL);

        painter->setPen(pen);
        painter->setBrush(brush);
        painter->drawRoundedRect(sizeRect(), radius, radius);
    }

    if(isSelected() && allowMouseResize()) {
        paintResizeHandles(*painter);
    }

    if(isSelected() && allowMouseRotate()) {
        paintRotateHandle(*painter);
    }
}

void BaseBlock::contextMenuEvent(QGraphicsSceneContextMenuEvent *event) {
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

        QAction *alignConnectorLabels = new QAction;
        alignConnectorLabels->setText("Align connector labels");
        connect(alignConnectorLabels, &QAction::triggered, [this] {
            this->alignConnectorLabels();
        });

        QAction *showAllConnectors = new QAction;
        showAllConnectors->setText("Show all connectors");
        connect(showAllConnectors, &QAction::triggered, [this] {
            if(!scene())
                return;

            for(const std::shared_ptr<Items::Connector> &conn : connectors())
                scene()->undoStack()->push(new QSchematic::Commands::ItemVisibility(conn, true));
        });

        //QAction *newConnector = new QAction;
        //newConnector->setText("Add connector");
        //connect(newConnector, &QAction::triggered, [this, event] {
        //    if(!scene())
        //        return;

        //    auto connector = std::make_shared<BaseBlockConnector>(event->pos().toPoint(), QStringLiteral("Unnamed"), this);

        //    scene()->undoStack()->push(new ::Commands::NodeAddConnector(this, connector));
        //});

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
        //menu.addAction(newConnector);
        menu.addAction(alignConnectorLabels);
        menu.addAction(showAllConnectors);
        menu.addSeparator();
        menu.addAction(duplicate);
        menu.addAction(deleteFromModel);
        menu.addSeparator();
        menu.addAction(isMovable);
    }

    menu.exec(event->screenPos());
}

void BaseBlock::alignLabel() {
    if(!_label)
        return;

    const QRectF &tr = _label->textRect();
    const qreal x = (width() - tr.width()) / 2.0;
    qreal y = -10.0;

    _label->setPos(x, y);
}

std::shared_ptr<QSchematic::Items::Label> BaseBlock::label() const {
    return _label;
}

void BaseBlock::setText(const QString &text) {
    Q_ASSERT(_label);

    _label->setText(text);
}

QString BaseBlock::text() const {
    Q_ASSERT(_label);

    return _label->text();
}

Solver::BlockType BaseBlock::getSolverBlockType() const {
    return {
        QStringLiteral("Base"),
        0,
        0,
        0,

        false //Only true for blocks that doesn't cause algebraic loops
    };
}

void BaseBlock::solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) {

}

void BaseBlock::solveDerivative(const QVector<Solver::Signal> &in, const QVector<Solver::Signal> &states, QVector<Solver::Signal> &dx) {

}

void BaseBlock::copyAttributes(BaseBlock &dest) const {
    QSchematic::Items::Node::copyAttributes(dest);

    dest._label = std::dynamic_pointer_cast<QSchematic::Items::Label>(_label->deepCopy());
    dest._label->setParentItem(&dest);
    dest._label->setText(getUnusedName(dest._label->text()));
}

QString BaseBlock::getUnusedName(QString baseName) const {
    if(!nameIsInUse(baseName)) return baseName;

    QString returnName = QStringLiteral("%1 %2").arg(baseName, "%1");
    for(int i = 0; ; i++) {
        auto newName = returnName.arg(i);
        if(!nameIsInUse(newName)) return newName;
    }
}

bool BaseBlock::nameIsInUse(QString name) const {
    auto _instance = Windows::MainWindow::instance();
    if(!_instance)
        return false;

    auto _scene = _instance->scene();
    if(!_scene)
        return false;

    for(auto &node : _scene->nodes()) {
        auto baseNode = dynamic_cast<BaseBlock *>(node.get());
        if(baseNode == nullptr) continue;

        if(baseNode->text() == name) return true;
    }

    return false;
}

void BaseBlock::setupConnectors(QVector<BaseBlock::ConnectorAttribute> &connectorAttributes) {
    for(const auto &c : connectorAttributes) {
        auto connector = std::make_shared<BaseBlockConnector>(c.point, c.name, c.input, c.index);
        connector->label()->setVisible(false);
        addConnector(connector);
    }
}

BaseBlockConnector *BaseBlock::getConnector(bool input, int index) {
    for(auto &c : connectors()) {
        auto bc = dynamic_cast<BaseBlockConnector *>(c.get());
        if(!bc) continue;

        if(input == bc->isInput() && bc->index() == index) return bc;
    }

    return nullptr;
}

void BaseBlock::addProperty(const Properties::BlockProperty &property) {
    _properties.append(property);
}

void BaseBlock::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    Properties::PropertiesDialog dlg(this);

    if(dlg.exec() == QDialog::Accepted) {
        //TODO: Update
    }

    QSchematic::Items::Node::mouseDoubleClickEvent(event);
}
