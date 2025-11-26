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

        virtual Solver::BlockType getSolverBlockType();
        virtual QVector<double> getSolverParams();

    protected:
        void copyAttributes(BaseBlock &dest) const;

    private:
        std::shared_ptr<QSchematic::Items::Label> _label;
};

}