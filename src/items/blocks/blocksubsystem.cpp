#include <items/blocks/blocksubsystem.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/itemtypes.hpp>
#include <windows/subsystemwindow.hpp>

#include <items/blocks/blockinput.hpp>
#include <items/blocks/blockoutput.hpp>

#include <qschematic/scene.hpp>
#include <qschematic/view.hpp>
#include <qschematic/items/label.hpp>
#include <qschematic/commands/item_remove.hpp>
#include <qschematic/commands/item_visibility.hpp>
#include <qschematic/commands/label_rename.hpp>
#include <qschematic/netlist.hpp>

#include <QMenu>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QGraphicsDropShadowEffect>

using namespace Blocks;

BlockSubsystem::BlockSubsystem(Windows::BaseWindow *window, QGraphicsItem *parent) :
    BaseBlock(::ItemType::BlockSubsystemType, window, parent)
{
    setSize(80, 80);
    setBaseName(QStringLiteral("Subsystem"));

    //QVector<ConnectorAttribute> connectorAttributes = {
    //    { true, 0, QPoint(0, 2), QStringLiteral("in") }
    //};
    //setupConnectors(connectorAttributes);

    setDescription(QStringLiteral("Subsystem description TODO"));

    //setConnectorsMovable(false);
}

gpds::container BlockSubsystem::to_container() const {
    gpds::container root;
    addItemTypeIdToContainer(root);
    root.add_value("operation", BaseBlock::to_container());
    if(_subsystemWindow != nullptr) root.add_value("scene", _subsystemWindow->to_container());
    root.add_value("inputs", _inputs);
    root.add_value("outputs", _outputs);

    return root;
}

void BlockSubsystem::from_container(const gpds::container &container) {
    BaseBlock::from_container(*container.get_value<gpds::container *>("operation").value());
    auto tempScene = container.get_value<gpds::container *>("scene").value_or(nullptr);
    if(tempScene != nullptr) {
        _subsystemSceneContainer = *tempScene;
        _subsystemSceneContainerValid = true;

        generateSubsystemWindow();
    }
    _inputs = container.get_value<int>("inputs").value_or(0);
    _outputs = container.get_value<int>("outputs").value_or(0);
}

std::shared_ptr<QSchematic::Items::Item> BlockSubsystem::deepCopy() const {
    auto clone = std::make_shared<BlockSubsystem>(parentWindow(), parentItem());
    copyAttributes(*clone);

    return clone;
}

void BlockSubsystem::copyAttributes(BlockSubsystem &dest) const {
    BaseBlock::copyAttributes(dest);
}

Solver::BlockType BlockSubsystem::getSolverBlockType() const {
    using namespace std::placeholders;
    return {
        QStringLiteral("subsystem"),
        _inputs, //Inputs
        _outputs, //Outputs
        0, //States

        false //Only true for blocks that doesn't cause algebraic loops
    };
}

void BlockSubsystem::mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event) {
    generateSubsystemWindow();

    _subsystemWindow->raise();
    _subsystemWindow->activateWindow();
    _subsystemWindow->show();

    BaseBlock::Node::mouseDoubleClickEvent(event);
}

void BlockSubsystem::generateSubsystemWindow() {
    if(_subsystemWindow == nullptr) {
        _subsystemWindow = new Windows::SubsystemWindow(this);

        if(_subsystemSceneContainerValid) _subsystemWindow->from_container(_subsystemSceneContainer);
    }
}

void BlockSubsystem::recalculateInputsOutputs() {
    _inputs = 0;
    _outputs = 0;
    if(_subsystemWindow == nullptr) return;

    auto scene = _subsystemWindow->scene();

    //Update inputs/outputs on parent here
    for(auto &n : scene->nodes()) {
        auto *nodePtr = n.get();
        auto *nodeBlockPtr = dynamic_cast<Blocks::BaseBlock *>(nodePtr);
        if(nodeBlockPtr == nullptr) continue;

        //TODO: Filter inputs/outputs
        auto *nodeBlockInputPtr = dynamic_cast<Blocks::BlockInput *>(nodeBlockPtr);
        auto *nodeBlockOutputPtr = dynamic_cast<Blocks::BlockOutput *>(nodeBlockPtr);
        if(nodeBlockInputPtr != nullptr) {
            _inputs++;
        } else if(nodeBlockOutputPtr != nullptr) {
            _outputs++;
        }
    }

    auto minHeight = ((_inputs > _outputs) ? _inputs : _outputs) * 40;
    if(minHeight < 40) minHeight = 40;
    if(size().height() < minHeight) {
        setSize(QSizeF(size().width(), minHeight));
    }

    //This function reshapes the connector to look like this
    //|----------|
    //|-I0    O0-|
    //|          |
    //|-I1    O1-|
    //|          |
    //|-I2    O2-|
    //|----------|

    QVector<ConnectorAttribute> connectorAttributes;
    for(int i = 0; i < _inputs; i++) {
        connectorAttributes.append({ true, i, QPoint(0, i * 2 + 1), QStringLiteral("in %1").arg(i) });
    }
    for(int i = 0; i < _outputs; i++) {
        connectorAttributes.append({ false, i, QPoint(size().width() / 20, i * 2 + 1), QStringLiteral("out %1").arg(i) });
    }
    setupConnectors(connectorAttributes);
}

void BlockSubsystem::populateNetlist(QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector*> *netlist, QString parentPrefix) {
    if(_subsystemWindow == nullptr) return;

    auto innerNetlist = _subsystemWindow->generateNetlist(parentPrefix);

    for(auto &node : innerNetlist->nodes) {
        auto *blockBasePtr = dynamic_cast<Blocks::BaseBlock *>(node);
        if(blockBasePtr == nullptr) continue;

        blockBasePtr->setSolverPrefix(QStringLiteral(""));
    }

    for(auto &innerNet : innerNetlist->nets) {
        bool netMerged = false;

        for(auto &[innerConn, innerNode] : innerNet.connectorNodePairs) {
            if(!innerNode) continue;
            if(innerNode->scene() != _subsystemWindow->scene()) continue;

            if(auto *blockIn = dynamic_cast<Blocks::BlockInput *>(innerNode)) {
                auto externalConnector = getConnector(true, blockIn->index());
                if (!externalConnector)
                    continue;

                for(auto &externalNet : netlist->nets) {
                    bool found = false;
                    for(auto &[extConn, extNode] : externalNet.connectorNodePairs) {
                        if(extConn == externalConnector) {
                            found = true;
                            break;
                        }
                    }

                    if(found) {
                        externalNet.connectorNodePairs.insert(std::pair<Blocks::BaseBlockConnector*, Blocks::BaseBlock *>(blockIn->getConnector(true, 0), blockIn));
                        break;
                    }
                }
            } else if(auto *blockOut = dynamic_cast<Blocks::BlockOutput *>(innerNode)) {
                auto externalConnector = getConnector(false, blockOut->index());
                if (!externalConnector)
                    continue;

                for(auto &externalNet : netlist->nets) {
                    bool found = false;
                    for(auto &[extConn, extNode] : externalNet.connectorNodePairs) {
                        if(extConn == externalConnector) {
                            found = true;
                            break;
                        }
                    }

                    if(found) {
                        externalNet.connectorNodePairs.insert(std::pair<Blocks::BaseBlockConnector*, Blocks::BaseBlock *>(blockOut->getConnector(false, 0), blockOut));
                        break;
                    }
                }
            }
        }
    }

    for(auto &node : innerNetlist->nodes) {
        auto prefix = QStringLiteral("%1%2/").arg(parentPrefix, text());
        auto *blockBasePtr = dynamic_cast<Blocks::BaseBlock *>(node);
        auto *blockSubsystem = dynamic_cast<Blocks::BlockSubsystem *>(node);

        if(blockSubsystem != nullptr) {
            blockSubsystem->populateNetlist(innerNetlist, prefix);
        } else if(blockBasePtr != nullptr) {
            blockBasePtr->setSolverPrefix(prefix);
        }
    }

    netlist->nodes.insert(netlist->nodes.end(), innerNetlist->nodes.begin(), innerNetlist->nodes.end());
    netlist->nets.insert(netlist->nets.end(), innerNetlist->nets.begin(), innerNetlist->nets.end());
}
