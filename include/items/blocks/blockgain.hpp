#pragma once

#include <items/blocks/baseblock.hpp>

namespace Blocks {

class BlockGain : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockGain)

    public:
        explicit BlockGain(Windows::BaseWindow *window = nullptr, QGraphicsItem *parent = nullptr);
        ~BlockGain() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;
        void sizeChangedEvent(QSizeF oldSize, QSizeF newSize) override;
        void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = nullptr) override;

        Solver::BlockType getSolverBlockType() const override;
        void solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) override;

    private:
        double _gainValue = 1.0;

        void copyAttributes(BlockGain &dest) const;
};

}