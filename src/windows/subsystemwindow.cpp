#include <windows/subsystemwindow.hpp>
#include <items/customitemfactory.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/blocksubsystem.hpp>
#include <items/fancywire.hpp>
#include <items/widgets/dial.hpp>
#include <library/widget.hpp>
#include <netlist/widget.hpp>

#include <solver/simulationworker.hpp>

#include <gpds/archiver_xml.hpp>
#include <qschematic/scene.hpp>
#include <qschematic/view.hpp>
#include <qschematic/commands/item_add.hpp>
#include <qschematic/items/node.hpp>
#include <qschematic/items/itemfactory.hpp>
#include <qschematic/items/widget.hpp>
#include <qschematic/netlist.hpp>
#include <qschematic/netlistgenerator.hpp>

#include <QApplication>
#include <QThread>
#include <QToolBar>
#include <QAction>
#include <QActionGroup>
#include <QFile>
#include <QDir>
#include <QMenuBar>
#include <QMenu>
#include <QUndoView>
#include <QDockWidget>
#include <QGraphicsSceneContextMenuEvent>
#include <QInputDialog>
#include <QFileDialog>
#include <QtDebug>

#include <functional>
#include <memory>
#include <sstream>

using namespace Windows;

SubsystemWindow::SubsystemWindow(Blocks::BlockSubsystem *blockParent, QWidget *parent) :
    _parent(blockParent),
    BaseWindow(parent)
{
    connect(_scene, &QSchematic::Scene::netlistChanged, [this]() {
        //qDebug() << "Netlist changed";

        //generateNetlist();
    });

    setWindowTitle("SimBlock");
    resize(WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
#ifdef WINDOW_MAXIMIZE
    setWindowState(Qt::WindowMaximized);
#endif
}

gpds::container SubsystemWindow::to_container() const {
    return _scene->to_container();
}

void SubsystemWindow::from_container(const gpds::container &container) {
    _scene->from_container(container);

    //Update inputs/outputs on parent here
    for(auto &n : _scene->nodes()) {
        auto *nodePtr = n.get();
        auto *nodeBlockPtr = dynamic_cast<Blocks::BaseBlock *>(nodePtr);
        if(nodeBlockPtr == nullptr) continue;

        //TODO: Filter inputs/outputs
    }
}
