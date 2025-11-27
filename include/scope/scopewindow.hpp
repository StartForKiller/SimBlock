#pragma once

#include <QMainWindow>

class QCustomPlot;
class QCPGraph;

namespace Solver {
    struct Signal;
};

namespace Scope {

class ScopeWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ScopeWindow)

    public:
        explicit ScopeWindow(const QString &title, QWidget *parent = nullptr);
        ~ScopeWindow() override = default;

    private:
        QCustomPlot *_plot;
        QCPGraph *_graph;

    public slots:
        void onNewSample(double t, Solver::Signal value);
        void onStartSimulation();
};

}