#include <windows/subsystemwindow.hpp>
#include <items/customitemfactory.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/blocksubsystem.hpp>
#include <items/blocks/blockinput.hpp>
#include <items/blocks/blockoutput.hpp>
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

    connect(_scene, &QSchematic::Scene::itemAdded, [this](std::shared_ptr<QSchematic::Items::Item> item) {
        auto blockInputPtr = dynamic_cast<Blocks::BlockInput *>(item.get());
        auto blockOutputPtr = dynamic_cast<Blocks::BlockOutput *>(item.get());
        if(blockInputPtr != nullptr || blockOutputPtr != nullptr) _parent->recalculateInputsOutputs();
    });

    connect(_scene, &QSchematic::Scene::itemRemoved, [this](std::shared_ptr<QSchematic::Items::Item> item) {
        auto blockInputPtr = dynamic_cast<Blocks::BlockInput *>(item.get());
        auto blockOutputPtr = dynamic_cast<Blocks::BlockOutput *>(item.get());
        if(blockInputPtr != nullptr || blockOutputPtr != nullptr) _parent->recalculateInputsOutputs();
    });

    connect(_actionModeNormal, &QAction::triggered, [this]{
        _scene->setMode(QSchematic::Scene::NormalMode);
    });

    connect(_actionModeWire, &QAction::triggered, [this]{
        _scene->setMode(QSchematic::Scene::WireMode);
    });

    connect(_actionFitAll, &QAction::triggered, [this]{_view->fitInView();});

    connect(_actionClear, &QAction::triggered, [this]{
        Q_ASSERT(_scene);

        _scene->clear();
    });
}

gpds::container SubsystemWindow::to_container() const {
    return _scene->to_container();
}

void SubsystemWindow::from_container(const gpds::container &container) {
    _scene->from_container(container);

    _parent->recalculateInputsOutputs();
}

QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector*> *SubsystemWindow::generateNetlist(QString parentPrefix) {
    auto *netlist = new QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector*>();
    QSchematic::NetlistGenerator::generate(*netlist, *_scene);

    for(auto &net : netlist->nets) {
        net.name = QStringLiteral("%1%2/%3").arg(parentPrefix, _parent->text(), net.name);
    }

    return netlist;
}
