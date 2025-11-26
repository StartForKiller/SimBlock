#pragma once

#include <items/blocks/baseblock.hpp>

namespace Scope {
    class ScopeWindow;
}

class OperationScope : public Blocks::BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(OperationScope)

    public:
        explicit OperationScope(QGraphicsItem *parent = nullptr);
        ~OperationScope() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() override;

        void setInputNetName(QString name);
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    private:
        void generateScopeWindow();

        Scope::ScopeWindow *_scopeWindow = nullptr;

        QString _netName;

        void copyAttributes(OperationScope &dest) const;

    public slots:
        void onNewSample(double t, QMap<QString, double> values);
        void onStartSimulation();
};