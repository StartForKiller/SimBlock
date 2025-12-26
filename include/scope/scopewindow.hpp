#pragma once

#include <QMainWindow>

#include <scope/scopesignal.hpp>

class QCustomPlot;
class QCPGraph;

namespace Solver {
    struct Signal;
};

namespace Scope {

struct ChannelBinding {
    ScopeChannel *channel;
    QCPGraph *graph;
};

class ScopeWindow : public QMainWindow {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(ScopeWindow)

    public:
        explicit ScopeWindow(const QString &title, QWidget *parent = nullptr);
        ~ScopeWindow() override = default;

    private:
        double extract(const Solver::Signal &sig, const ChannelLayout &ch);
        void updatePlot();

        QCustomPlot *_plot;
        QCPGraph *_graph;

        QVector<ChannelLayout> _layout;
        QVector<ScopeChannel *> _channels;
        QVector<ChannelBinding> _bindings;

        QTimer *_refreshTimer;

    public slots:
        void onNewSample(double t, Solver::Signal value);
        void onStartSimulation(const Solver::Signal &definition);
};

}