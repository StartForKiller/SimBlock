#pragma once

#include <items/blocks/baseblock.hpp>

namespace Blocks {

class BlockDiv : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockDiv)

    public:
        explicit BlockDiv(QGraphicsItem *parent = nullptr);
        ~BlockDiv() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() const override;
        void solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) override;

    private:
        void copyAttributes(BlockDiv &dest) const;
};

}