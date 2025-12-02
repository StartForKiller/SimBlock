#pragma once

#include <items/blocks/baseblock.hpp>

namespace Blocks {

class BlockTF : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockTF)

    public:
        explicit BlockTF(QGraphicsItem *parent = nullptr);
        ~BlockTF() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() const override;
        void solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) override;
        void solveDerivative(const QVector<Solver::Signal> &in, const QVector<Solver::Signal> &states, QVector<Solver::Signal> &dx) override;

    private:
        void copyAttributes(BlockTF &dest) const;

        void updateMatrix();

        QVector<double> _numeratorCoefs;
        QVector<double> _denominatorCoefs;

        QVector<QVector<double>> _A;
        QVector<double> _B;
        QVector<double> _C;
        double _D;
};

}