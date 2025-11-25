#pragma once

#include <items/operation.hpp>

class OperationDemo1 : public Operation {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(OperationDemo1)

    public:
        explicit OperationDemo1(QGraphicsItem *parent = nullptr);
        ~OperationDemo1() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() override;

    private:
        void copyAttributes(OperationDemo1 &dest) const;

        void solveAlgebraic(const QMap<QString, double> &in, QMap<QString, double> &out, const QVector<double> &params, const QVector<double> &states);
        void solveDerivative(const QMap<QString, double> &in, const QVector<double> &states, QVector<double> &dx, const QVector<double> &params);
};