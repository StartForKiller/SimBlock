#pragma once

#include <items/blocks/baseblock.hpp>

namespace Scope {
    class ScopeWindow;
}

namespace Blocks {

class BlockScope : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockScope)

    public:
        explicit BlockScope(Windows::BaseWindow *window = nullptr, QGraphicsItem *parent = nullptr);
        ~BlockScope() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() const override;

        void setInputNetName(QString name);
        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    private:
        void generateScopeWindow();

        Scope::ScopeWindow *_scopeWindow = nullptr;

        QString _netName;

        void copyAttributes(BlockScope &dest) const;

    public slots:
        void onNewSample(double t, QMap<QString, Solver::Signal> values);
        void onStartSimulation(const Solver::Signal &definition);
};

}