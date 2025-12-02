#pragma once

#include <items/blocks/baseblock.hpp>

namespace Blocks {

class BlockDerivator : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockDerivator)

    public:
        explicit BlockDerivator(QGraphicsItem *parent = nullptr);
        ~BlockDerivator() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() const override;
        void solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) override;
        void solveDerivative(const QVector<Solver::Signal> &in, const QVector<Solver::Signal> &states, QVector<Solver::Signal> &xdot) override;

    private:
        void copyAttributes(BlockDerivator &dest) const;

        double _N = 100.0;
};

}