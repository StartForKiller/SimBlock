#include <windows/mainwindow.hpp>
#include <items/customitemfactory.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>
#include <items/blocks/blockscope.hpp>
#include <items/blocks/blocksubsystem.hpp>
#include <items/fancywire.hpp>
#include <items/widgets/dial.hpp>
#include <library/widget.hpp>
#include <netlist/widget.hpp>
#include <windows/view.hpp>

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

static void InitResources() {
    Q_INIT_RESOURCE(icons);
}

using namespace Windows;

const QString FILE_FILTERS = "XML (*.xml)";

static MainWindow *_instance;

MainWindow::MainWindow(QWidget *parent) : BaseWindow(parent) {
    InitResources();

    _instance = this;

    connect(_scene, &QSchematic::Scene::netlistChanged, [this]() {
        //qDebug() << "Netlist changed";

        //generateNetlist();
    });

    _netlistViewerWidget = new ::Netlist::Widget(this);
    QDockWidget *netlistViewerDockWidget = new QDockWidget;
    netlistViewerDockWidget->setWindowTitle(QStringLiteral("Netlist Viewer"));
    netlistViewerDockWidget->setWidget(_netlistViewerWidget);
    addDockWidget(Qt::LeftDockWidgetArea, netlistViewerDockWidget);

    setWindowTitle("SimBlock");
    resize(WINDOW_WIDTH, WINDOW_HEIGHT);
#ifdef WINDOW_MAXIMIZE
    setWindowState(Qt::WindowMaximized);
#endif

    connectActions();

    _simulationWorkerThread = new QThread();
    _simulationWorker = new Solver::SimulationWorker();
    _simulationWorker->moveToThread(_simulationWorkerThread);

    connect(_simulationWorker, &Solver::SimulationWorker::sampleGenerated, this, [this](double t, QMap<QString, Solver::Signal> values) {
        for(const auto& node : _scene->nodes()) {
            if(!node) continue;

            auto scopeNode = dynamic_cast<Blocks::BlockScope *>(node.get());
            if(scopeNode != nullptr) {
                scopeNode->onNewSample(t, values);
            }
        }
    });
    connect(_simulationWorkerThread, &QThread::started, _simulationWorker, &Solver::SimulationWorker::simulate);
    connect(_simulationWorker, &Solver::SimulationWorker::simulationFinished, this, [this]() {
        _simulationWorkerThread->quit();
        _simulating = false;
    });

    main();
}

bool MainWindow::save() {
    const std::filesystem::path path = QFileDialog::getSaveFileName(this, "Save to file", QDir::homePath(), FILE_FILTERS).toStdString();
    if(path.empty())
        return false;

    const auto &[success, message] = gpds::to_file<gpds::archiver_xml>(path, *_scene);
    if(!success) {
        qWarning() << "could not save to file: " << QString::fromStdString(message);
        return false;
    }

    return true;
}

bool MainWindow::load() {
    QString path = QFileDialog::getOpenFileName(this, "Load from file", QDir::homePath(), FILE_FILTERS);
    if(path.isEmpty())
        return false;

    return load(path);
}

bool MainWindow::load(const QString &filepath) {
    _scene->clear();

    QFile file(filepath);
    file.open(QFile::ReadOnly);
    if(!file.isOpen())
        return false;

    std::stringstream stream;
    stream << file.readAll().data();

    const auto &[success, message] = gpds::from_stream<gpds::archiver_xml>(stream, *_scene, QSchematic::Scene::gpds_name);
    if(!success) {
        qDebug() << "MainWindow::load(): Could not load scene: " << QString::fromStdString(message);
        return false;
    }

    file.close();

    return true;
}

void MainWindow::main() {
    _scene->clear();
    _scene->setSceneRect(-500, -500, 3000, 3000);
}

void MainWindow::connectActions() {
    connect(_actionOpen, &QAction::triggered, [this]{
        load();
    });

    connect(_actionSave, &QAction::triggered, [this]{
        save();
    });

    connect(_actionPrint, &QAction::triggered, [this]{
        //TODO
    });

    connect(_actionModeNormal, &QAction::triggered, [this]{
        _scene->setMode(QSchematic::Scene::NormalMode);
    });

    connect(_actionModeWire, &QAction::triggered, [this]{
        _scene->setMode(QSchematic::Scene::WireMode);
    });

    connect(_actionShowGrid, &QAction::toggled, [this](bool checked){
        _settings.showGrid = checked;
        settingsChanged();
    });

    connect(_actionFitAll, &QAction::triggered, [this]{_view->fitInView();});

    connect(_actionRouteStraightAngles, &QAction::toggled, [this](bool checked){
        _settings.routeStraightAngles = checked;
        _settings.preserveStraightAngles = checked;
        settingsChanged();
    });

    connect(_actionGenerateNetlist, &QAction::triggered, [this]{
        generateNetlist();
    });

    connect(_actionSolve, &QAction::triggered, [this]{
        solve();
    });

    connect(_actionTimeStep, &QAction::triggered, [this] {
        bool ok = false;
        const double newDouble = QInputDialog::getDouble(
            nullptr,
            "Set TimeStep Value",
            "New timestep value",
            _timeStep,
            0.000000001, 2147483647, 5,
            &ok
        );
        if(!ok)
            return;

        _timeStep = newDouble;
    });

    connect(_actionTimeToSimulate, &QAction::triggered, [this] {
        bool ok = false;
        const double newDouble = QInputDialog::getDouble(
            nullptr,
            "Set Run Time Value",
            "New run time value",
            _timeToSimulate,
            0, 2147483647, 5,
            &ok
        );
        if(!ok)
            return;

        _timeToSimulate = newDouble;
    });

    connect(_actionClear, &QAction::triggered, [this]{
        Q_ASSERT(_scene);

        _scene->clear();
    });

    connect(_actionDebugMode, &QAction::toggled, [this](bool checked){
        _settings.debug = checked;
        settingsChanged();
    });
}

void MainWindow::generateNetlist() {
    auto *netlist = new QSchematic::Netlist<Blocks::BaseBlock *, Blocks::BaseBlockConnector*>();
    QSchematic::NetlistGenerator::generate(*netlist, *_scene);

    auto netlistTemp = *netlist;
    for(auto &node : netlistTemp.nodes) {
        auto *blockSubsystem = dynamic_cast<Blocks::BlockSubsystem *>(node);
        if(blockSubsystem == nullptr) continue;

        blockSubsystem->populateNetlist(netlist, QStringLiteral(""));
    }

    _simulationWorker->setNetlist(netlist);

    Q_ASSERT(_netlistViewerWidget);
    _netlistViewerWidget->setNetlist(*netlist);
}

void MainWindow::solve() {
    if(_simulating) return;
    generateNetlist();

    for(const auto& node : _scene->nodes()) {
        if(!node) continue;

        auto scopeNode = dynamic_cast<Blocks::BlockScope *>(node.get());
        if(scopeNode != nullptr) {
            scopeNode->onStartSimulation(Solver::make_signal(0.0)); //TODO
        }
    }

    _simulationWorker->setTimeParameters(_timeStep, _timeToSimulate);

    _simulating = true;
    _simulationWorkerThread->start();
}

MainWindow *MainWindow::instance() {
    return _instance;
}
