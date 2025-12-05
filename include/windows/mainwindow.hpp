#pragma once

#include <qschematic/settings.hpp>

#include <windows/basewindow.hpp>
#include <memory>

class QUndoView;
class QGraphicsSceneContextMenuEvent;

namespace QSchematic {
    class Scene;
    class View;
};

namespace Library {
    class Widget;
};

namespace Netlist
{
    class Widget;
};

namespace Solver
{
    class SimulationWorker;
};

namespace Windows {

class MainWindow : public BaseWindow {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(MainWindow)

    public:
        explicit MainWindow(QWidget *parent = nullptr);
        ~MainWindow() override = default;

        bool save();
        bool load();
        bool load(const QString &filepath);
        void main();

        void connectActions();

    public:
        static MainWindow *instance();

    private:
        void generateNetlist();
        void solve();

        ::Netlist::Widget* _netlistViewerWidget = nullptr;

        bool _simulating = false;
        QThread* _simulationWorkerThread;
        Solver::SimulationWorker *_simulationWorker = nullptr;
        double _timeStep = 0.01;
        double _timeToSimulate = 5.0;
};

}