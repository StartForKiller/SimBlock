#pragma once

#include <qschematic/netlist.hpp>

#include <QWidget>

namespace Blocks {
    class BaseBlock;
    class BaseBlockConnector;
};

namespace Netlist {

class Model;
class View;

class Viewer : public QWidget {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Viewer)

    public:
        Viewer(QWidget *parent = nullptr);
        ~Viewer() override = default;

        void setNetlist(const QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> &netlist);

    private:
        Model *_model = nullptr;
        View *_view = nullptr;
};

}