#pragma once

#include <qschematic/settings.hpp>
#include <qschematic/netlist.hpp>
#include <gpds/container.hpp>

#include <windows/basewindow.hpp>
#include <memory>

namespace Blocks {
    class BlockSubsystem;
    class BaseBlock;
    class BaseBlockConnector;
};

namespace Windows {

class SubsystemWindow : public BaseWindow {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(SubsystemWindow)

    public:
        explicit SubsystemWindow(Blocks::BlockSubsystem *blockParent, QWidget *parent = nullptr);
        ~SubsystemWindow() override = default;

        gpds::container to_container() const;
        void from_container(const gpds::container &container);

        QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector*> *generateNetlist(QString parentPrefix);

    private:
        Blocks::BlockSubsystem *_parent;
};

}