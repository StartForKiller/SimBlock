#pragma once

#include <items/operation.hpp>

class OperationConstant : public Operation {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(OperationConstant)

    public:
        explicit OperationConstant(QGraphicsItem *parent = nullptr);
        ~OperationConstant() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;
        void contextMenuEvent(QGraphicsSceneContextMenuEvent *event) override;

        Solver::BlockType getSolverBlockType() override;

    private:
        double _constantValue;

        void copyAttributes(OperationConstant &dest) const;

        void solveAlgebraic(const QMap<QString, double> &in, QMap<QString, double> &out, const QVector<double> &params, const QVector<double> &states);
};