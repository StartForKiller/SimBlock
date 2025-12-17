#pragma once

#include <items/blocks/baseblock.hpp>

namespace Blocks {

class BlockSum : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockSum)

    public:
        explicit BlockSum(Windows::BaseWindow *window = nullptr, QGraphicsItem *parent = nullptr);
        ~BlockSum() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;
        void sizeChangedEvent(QSizeF oldSize, QSizeF newSize) override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

        Solver::BlockType getSolverBlockType() const override;
        void solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) override;

    private:
        void copyAttributes(BlockSum &dest) const;

        void updateConnectors();

        QString _descriptorString = QStringLiteral("|++");
        int _inputs = 2;
        double _gains[3];
};

}