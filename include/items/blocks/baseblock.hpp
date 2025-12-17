#pragma once

#include <items/itemtypes.hpp>
#include <solver/solver.hpp>

#include <items/blocks/properties/blockproperty.hpp>

#include <qschematic/items/node.hpp>

#include <QVector>

namespace QSchematic::Items {
    class Label;
};

namespace Windows {
    class BaseWindow;
}

namespace Blocks {

class BaseBlock : public QSchematic::Items::Node {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BaseBlock)

    public:
        struct ConnectorAttribute {
            bool input;
            int index;
            QPoint point;
            QString name;
        };

    public:
        explicit BaseBlock(int type = ::ItemType::BaseBlockType, Windows::BaseWindow *window = nullptr, QGraphicsItem *parent = nullptr);
        ~BaseBlock() override;

        Windows::BaseWindow *parentWindow() const { return _window; }

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;
        std::unique_ptr<QWidget> popup() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

        BaseBlockConnector *getConnector(bool input, int index);

        void alignLabel();
        std::shared_ptr<QSchematic::Items::Label> label() const;
        void setText(const QString &text);
        void setSolverPrefix(QString prefix) { _solverPrefix = prefix; }
        QString solverName();
        QString text() const;
        QString baseName() const { return _baseName; }
        QString description() const { return _description; }

        void mirrorHorizontal();
        void mirrorVertical();
        bool isMirrored() { return _mirrored; }
        int mirrorOrientation() { return _mirrorOrientation; }
        void rotateHandler();

        virtual Solver::BlockType getSolverBlockType() const;

        virtual void solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states);
        virtual void solveDerivative(const QVector<Solver::Signal> &in, const QVector<Solver::Signal> &states, QVector<Solver::Signal> &dx);

    protected:
        void copyAttributes(BaseBlock &dest) const;

        void setBaseName(QString baseName);
        void setDescription(QString description) { _description = description; }

        QString getUnusedName(QString baseName) const;
        bool nameIsInUse(QString name) const;

        void addProperty(const Properties::BlockProperty &property);

        void setupConnectors(QVector<ConnectorAttribute> &connectorAttributes);

    public:
        const QVector<Properties::BlockProperty> &properties() const { return _properties; }
        QVector<Properties::BlockProperty> &properties() { return _properties; }

    private:
        Windows::BaseWindow *_window;

        std::shared_ptr<QSchematic::Items::Label> _label;

        bool _mirrored = false;
        uint _mirrorOrientation = 0;
        QAction *_actionRotate = nullptr;

        QString _baseName;
        QString _solverPrefix = QStringLiteral("");
        QString _description;
        QVector<Properties::BlockProperty> _properties;
};

}