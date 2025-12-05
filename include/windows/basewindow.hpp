#pragma once

#include <qschematic/settings.hpp>

#include <QMainWindow>
#include <memory>

#ifndef WINDOW_WIDTH
    #define WINDOW_WIDTH 1920
#endif
#ifndef WINDOW_HEIGHT
    #define WINDOW_HEIGHT 1080
#endif

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

class BaseWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(BaseWindow)

    public:
        explicit BaseWindow(QWidget *parent = nullptr);
        ~BaseWindow() override = default;

        void createActions();

        void setSettings(QSchematic::Settings settings);

    public:
        QSchematic::Scene *scene();

    protected:
        void settingsChanged();

        QSchematic::Scene *_scene = nullptr;
        QSchematic::View *_view = nullptr;
        QSchematic::Settings _settings;
        Library::Widget *_itemLibraryWidget = nullptr;
        QUndoView* _undoView = nullptr;

        QAction *_actionOpen = nullptr;
        QAction *_actionSave = nullptr;
        QAction *_actionPrint = nullptr;
        QAction *_actionUndo = nullptr;
        QAction *_actionRedo = nullptr;
        QAction *_actionModeNormal = nullptr;
        QAction *_actionModeWire = nullptr;
        QAction *_actionShowGrid = nullptr;
        QAction *_actionFitAll = nullptr;
        QAction *_actionRouteStraightAngles = nullptr;
        QAction *_actionGenerateNetlist = nullptr;
        QAction *_actionClear = nullptr;
        QAction *_actionDebugMode = nullptr;

        QAction *_actionSolve = nullptr;
        QAction *_actionTimeStep = nullptr;
        QAction *_actionTimeToSimulate = nullptr;
};

}