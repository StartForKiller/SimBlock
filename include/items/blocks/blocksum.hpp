#pragma once

#include <items/blocks/baseblock.hpp>

namespace Blocks {

class BlockSum : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockSum)

    public:
        explicit BlockSum(QGraphicsItem *parent = nullptr);
        ~BlockSum() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() const override;
        void solveAlgebraic(const QVector<double> &in, QVector<double> &out, const QVector<double> &params, const QVector<double> &states) override;

    private:
        void copyAttributes(BlockSum &dest) const;
};

}