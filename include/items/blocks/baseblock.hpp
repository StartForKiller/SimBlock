#pragma once

#include <items/itemtypes.hpp>
#include <solver/solver.hpp>

#include <qschematic/items/node.hpp>

#include <QVector>

namespace QSchematic::Items {
    class Label;
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
        explicit BaseBlock(int type = ::ItemType::BaseBlockType, QGraphicsItem *parent = nullptr);
        ~BaseBlock() override;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;
        std::unique_ptr<QWidget> popup() const override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

        void alignLabel();
        std::shared_ptr<QSchematic::Items::Label> label() const;
        void setText(const QString &text);
        QString text() const;

        virtual Solver::BlockType getSolverBlockType() const;
        virtual QVector<double> getSolverParams();

        virtual void solveAlgebraic(const QVector<double> &in, QVector<double> &out, const QVector<double> &params, const QVector<double> &states);
        virtual void solveDerivative(const QVector<double> &in, const QVector<double> &states, QVector<double> &dx, const QVector<double> &params);

    protected:
        void copyAttributes(BaseBlock &dest) const;

        QString getUnusedName(QString baseName) const;
        bool nameIsInUse(QString name) const;

        void setupConnectors(QVector<ConnectorAttribute> &connectorAttributes);

    private:
        std::shared_ptr<QSchematic::Items::Label> _label;
};

}