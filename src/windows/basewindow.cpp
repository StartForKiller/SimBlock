#include <windows/basewindow.hpp>
#include <items/customitemfactory.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/blockscope.hpp>
#include <items/fancywire.hpp>
#include <items/widgets/dial.hpp>
#include <library/widget.hpp>
#include <netlist/widget.hpp>

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

BaseWindow::BaseWindow(QWidget *parent) : QMainWindow(parent) {
    auto func = std::bind(&CustomItemFactory::from_container, std::placeholders::_1);
    QSchematic::Items::Factory::instance().setCustomItemsFactory(func);

    _settings.debug = false;
    _settings.showGrid = false;
    _settings.routeStraightAngles = true;

    _scene = new QSchematic::Scene(this);

    createActions();

    _scene->setSettings(_settings);
    _scene->setWireFactory([]{ return std::make_shared<FancyWire>(); });
    connect(_scene, &QSchematic::Scene::modeChanged, [this](int mode){
        switch(mode) {
            case QSchematic::Scene::NormalMode:
                _actionModeNormal->setChecked(true);

            case QSchematic::Scene::WireMode:
                _actionModeWire->setChecked(true);

            default:
                break;
        }
    });

    _view = new QSchematic::View(this);
    _view->setSettings(_settings);
    _view->setScene(_scene);

    _itemLibraryWidget = new Library::Widget(this);
    connect(_view, &QSchematic::View::zoomChanged, _itemLibraryWidget, &Library::Widget::setPixmapScale);
    QDockWidget *itemLibraryDock = new QDockWidget;
    itemLibraryDock->setWindowTitle("Items");
    itemLibraryDock->setWidget(_itemLibraryWidget);
    addDockWidget(Qt::LeftDockWidgetArea, itemLibraryDock);

    _undoView = new QUndoView(_scene->undoStack(), this);
    QDockWidget* undoDockWiget = new QDockWidget;
    undoDockWiget->setWindowTitle("Command History");
    undoDockWiget->setWidget(_undoView);
    addDockWidget(Qt::LeftDockWidgetArea, undoDockWiget);

    {
        QMenu *fileMenu = new QMenu(QStringLiteral("&File"), this);
        fileMenu->addAction(_actionOpen);
        fileMenu->addAction(_actionSave);
        fileMenu->addSeparator();
        fileMenu->addAction(_actionPrint);

        QMenuBar *menuBar = new QMenuBar(this);
        menuBar->addMenu(fileMenu);
        setMenuBar(menuBar);
    }

    QToolBar *editorToolbar = new QToolBar(this);
    editorToolbar->addAction(_actionUndo);
    editorToolbar->addAction(_actionRedo);
    editorToolbar->addSeparator();
    editorToolbar->addAction(_actionModeNormal);
    editorToolbar->addAction(_actionModeWire);
    editorToolbar->addSeparator();
    editorToolbar->addAction(_actionRouteStraightAngles);
    editorToolbar->addSeparator();
    editorToolbar->addAction(_actionGenerateNetlist);
    editorToolbar->addAction(_actionClear);
    editorToolbar->addSeparator();
    editorToolbar->addAction(_actionTimeStep);
    editorToolbar->addAction(_actionTimeToSimulate);
    editorToolbar->addAction(_actionSolve);
    addToolBar(editorToolbar);

    QToolBar *viewToolbar = new QToolBar(this);
    viewToolbar->addAction(_actionShowGrid);
    viewToolbar->addAction(_actionFitAll);
    addToolBar(viewToolbar);

    QToolBar *debugToolbar = new QToolBar(this);
    debugToolbar->addAction(_actionDebugMode);
    addToolBar(debugToolbar);

    setCentralWidget(_view);

    setWindowTitle("SimBlock");
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
}

void BaseWindow::createActions() {
    _actionOpen = new QAction(this);
    _actionOpen->setText("Open");
    _actionOpen->setIcon(QIcon(":/folder_open.svg"));
    _actionOpen->setToolTip("Open a file");
    _actionOpen->setShortcut(QKeySequence::Open);

    _actionSave = new QAction(this);
    _actionSave->setText("Save");
    _actionSave->setIcon(QIcon(":/save.svg"));
    _actionSave->setToolTip("Save to a file");
    _actionSave->setShortcut(QKeySequence::Save);

    _actionPrint = new QAction(this);
    _actionPrint->setText("Print");
    _actionPrint->setIcon(QIcon(":/print.svg"));
    _actionPrint->setShortcut(QKeySequence::Print);

    _actionUndo = _scene->undoStack()->createUndoAction(this, QStringLiteral("Undo"));
    _actionUndo->setIcon(QIcon(":/undo.svg"));
    _actionUndo->setText("Undo");
    _actionUndo->setShortcut(QKeySequence::Undo);

    _actionRedo = _scene->undoStack()->createRedoAction(this, QStringLiteral("Redo"));
    _actionRedo->setIcon(QIcon(":/redo.svg"));
    _actionRedo->setText("Redo");
    _actionRedo->setShortcut(QKeySequence::Redo);

    _actionModeNormal = new QAction("Normal Mode", this);
    _actionModeNormal->setIcon(QIcon(":/mode_normal.svg"));
    _actionModeNormal->setToolTip("Change to the normal mode (allows to move components).");
    _actionModeNormal->setCheckable(true);
    _actionModeNormal->setChecked(true);

    _actionModeWire = new QAction("Wire Mode", this);
    _actionModeWire->setIcon(QIcon(":/mode_wire.svg"));
    _actionModeWire->setToolTip("Change to the wire mode (allows to draw wires).");
    _actionModeWire->setCheckable(true);

    QActionGroup *actionGroupMode = new QActionGroup(this);
    actionGroupMode->addAction(_actionModeNormal);
    actionGroupMode->addAction(_actionModeWire);

    _actionShowGrid = new QAction("Toggle Grid", this);
    _actionShowGrid->setIcon(QIcon(":/grid.svg"));
    _actionShowGrid->setCheckable(true);
    _actionShowGrid->setChecked(_settings.showGrid);
    _actionShowGrid->setToolTip("Toggle grid visibility");

    _actionFitAll = new QAction("Fit All", this);
    _actionFitAll->setIcon(QIcon(":/fit_all.svg"));
    _actionFitAll->setToolTip("Center view on all items");

    _actionRouteStraightAngles = new QAction("Wire angles", this);
    _actionRouteStraightAngles->setIcon(QIcon( ":/wire_rightangle.svg"));
    _actionRouteStraightAngles->setCheckable(true);
    _actionRouteStraightAngles->setChecked(_settings.routeStraightAngles);

    _actionGenerateNetlist = new QAction("Generate netlist", this);
    _actionGenerateNetlist->setIcon(QIcon(":/netlist.svg"));

    _actionSolve = new QAction("Solve", this);
    _actionSolve->setIcon(QIcon(":/run.svg"));

    _actionTimeStep = new QAction("Timestep", this);
    _actionTimeStep->setText("Timestep ...");

    _actionTimeToSimulate = new QAction("Run Time", this);
    _actionTimeToSimulate->setText("Run Time ...");

    _actionClear = new QAction("Clear", this);
    _actionClear->setIcon(QIcon(":/clean.svg"));

    _actionDebugMode = new QAction("Debug", this);
    _actionDebugMode->setCheckable(true);
    _actionDebugMode->setIcon(QIcon(":/bug.svg"));
    _actionDebugMode->setChecked(_settings.debug);
}

void BaseWindow::setSettings(QSchematic::Settings settings) {
    _settings = settings;
    settingsChanged();
}

void BaseWindow::settingsChanged() {
    _view->setSettings(_settings);
    _scene->setSettings(_settings);
}

QSchematic::Scene *BaseWindow::scene() {
    return _scene;
}
