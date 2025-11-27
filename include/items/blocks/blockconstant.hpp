#pragma once

#include <items/blocks/baseblock.hpp>

namespace Blocks {

class BlockConstant : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockConstant)

    public:
        explicit BlockConstant(QGraphicsItem *parent = nullptr);
        ~BlockConstant() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() const override;
        void solveAlgebraic(const QVector<double> &in, QVector<double> &out, const QVector<double> &params, const QVector<double> &states) override;

    private:
        double _constantValue = 0.0;

        void copyAttributes(BlockConstant &dest) const;
};

}