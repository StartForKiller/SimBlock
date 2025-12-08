#pragma once

#include <items/blocks/baseblock.hpp>

namespace Blocks {

class BlockInput : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockInput)

    public:
        explicit BlockInput(Windows::BaseWindow *window = nullptr, QGraphicsItem *parent = nullptr);
        ~BlockInput() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() const override;
        void solveAlgebraic(const QVector<Solver::Signal> &in, QVector<Solver::Signal> &out, const QVector<Solver::Signal> &states) override;

        int index() { return _index; }

    private:
        void copyAttributes(BlockInput &dest) const;

        int _index = 0;
};

}