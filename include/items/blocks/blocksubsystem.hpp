#pragma once

#include <items/blocks/baseblock.hpp>

namespace Windows {
    class SubsystemWindow;
}

namespace Blocks {

class BlockSubsystem : public BaseBlock {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BlockSubsystem)

    public:
        explicit BlockSubsystem(QGraphicsItem *parent = nullptr);
        ~BlockSubsystem() override = default;

        gpds::container to_container() const override;
        void from_container(const gpds::container &container) override;
        std::shared_ptr<QSchematic::Items::Item> deepCopy() const override;

        Solver::BlockType getSolverBlockType() const override;

        void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) override;

    private:
        void copyAttributes(BlockSubsystem &dest) const;

        void generateSubsystemWindow();

        Windows::SubsystemWindow *_subsystemWindow = nullptr;
        gpds::container _subsystemSceneContainer;
        bool _subsystemSceneContainerValid = false;
        int _inputs = 0;
        int _outputs = 0;
};

}