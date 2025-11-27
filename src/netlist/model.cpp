#include <netlist/model.hpp>
#include <common/treeitem.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>

using namespace Netlist;
using namespace Blocks;

Model::Model(QObject *parent) :
    TreeModel(parent)
{
    _rootItem->setData({"Name", "Address"});
}

void Model::setNetlist(const QSchematic::Netlist<BaseBlock *, BaseBlockConnector *> &netlist) {
    clear();

    if(netlist.nets.empty())
        return;

    Q_ASSERT(_rootItem);
    beginInsertRows(QModelIndex(), 0, static_cast<int>(netlist.nets.size()) - 1);

    for(const auto &net : netlist.nets) {
        TreeItem *netItem = new TreeItem({net.name, pointerToString(&net)});
        _rootItem->appendChild(netItem);

        TreeItem *nodesItem = new TreeItem({ QStringLiteral("Nodes"), "" });
        netItem->appendChild(nodesItem);
        for(const auto &node : net.nodes) {
            Q_ASSERT(node);
            TreeItem *nodeItem = new TreeItem({node->text(), pointerToString(&node)});
            nodesItem->appendChild(nodeItem);
        }

        TreeItem *connectorsItem = new TreeItem({ QStringLiteral("Connectors"), "" });
        netItem->appendChild(connectorsItem);
        for(const auto &connector : net.connectors) {
            Q_ASSERT(connector);
            Q_ASSERT(connector->label());
            TreeItem *connectorItem = new TreeItem({connector->label()->text(), pointerToString(connector)});
            connectorsItem->appendChild(connectorItem);
        }

        TreeItem *wiresItem = new TreeItem({ QStringLiteral("Wires"), "" });
        netItem->appendChild(wiresItem);
        for(const auto &wire : net.wires) {
            Q_ASSERT(wire);
            TreeItem *wireItem = new TreeItem({"Wire", pointerToString(wire)});
            wiresItem->appendChild(wireItem);
        }
    }

    endInsertRows();
}

QString Model::pointerToString(const void *ptr) {
    return QString("0x%1").arg((quintptr)ptr, QT_POINTER_SIZE * 2, 16, QChar('0'));
}