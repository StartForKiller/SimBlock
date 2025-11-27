#pragma once

#include <qschematic/netlist.hpp>

#include <common/treemodel.hpp>

namespace Blocks {
    class BaseBlock;
    class BaseBlockConnector;
};

namespace Netlist {

class Model : public TreeModel {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(Model)

    public:
        explicit Model(QObject *parent = nullptr);
        ~Model() override = default;

        void setNetlist(const QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector *> &netlist);

    private:
        [[nodiscard]] QString pointerToString(const void *ptr);
};

}