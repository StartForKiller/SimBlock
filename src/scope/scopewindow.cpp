#include <scope/scopewindow.hpp>
#include <solver/solver.hpp>

#include <qcustomplot.h>

#include <QBoxLayout>

using namespace Scope;

ScopeWindow::ScopeWindow(const QString &title, QWidget *parent) :
    QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    setWindowTitle(title);
    resize(900, 600);

    _plot = new QCustomPlot(this);
    _graph = _plot->addGraph();

    auto layout = new QVBoxLayout();
    layout->addWidget(_plot);

    auto window = new QWidget();
    window->setLayout(layout);

    setCentralWidget(window);

    _refreshTimer = new QTimer(this);
    connect(_refreshTimer, &QTimer::timeout, this, &ScopeWindow::updatePlot);
}

void ScopeWindow::onNewSample(double t, Solver::Signal value) {
    for(int i = 0; i < _layout.size(); i++) {
        double y = extract(value, _layout[i]);
        _channels[i]->push(t, y);

        //_graph->addData(t, y);
        //_plot->xAxis->setRange(_plot->xAxis->range().lower, t + 0.5);
        //if(y > (_plot->yAxis->range().upper - 1)) {
        //    _plot->yAxis->setRange(_plot->yAxis->range().lower, y + 1);
        //}
        //if(y < (_plot->yAxis->range().lower + 1)) {
        //    _plot->yAxis->setRange(y - 1, _plot->yAxis->range().upper);
        //}
        //_plot->replot(QCustomPlot::rpQueuedReplot);
        //updatePlot();
    }
}

void ScopeWindow::updatePlot() {
    for(auto &b : _bindings) {
        const auto &samples = b.channel->data();

        QVector<double> x, y;
        x.reserve(samples.size());
        y.reserve(samples.size());

        for(const auto &s : samples) {
            x.push_back(s.x);
            y.push_back(s.y);
        }

        b.graph->setData(x, y);
    }

    _plot->rescaleAxes(true);
    _plot->replot(QCustomPlot::rpQueuedReplot);
}

void ScopeWindow::onStartSimulation(const Solver::Signal &definition) {
    _refreshTimer->stop();

    _layout.clear();
    _channels.clear();
    _plot->clearGraphs();
    _bindings.clear();

    _layout = SignalAdapter::explode(definition, "test"); //TODO

    for(const auto &ch : _layout) {
        auto *channel = new ScopeChannel();
        channel->name = ch.name;

        _channels.push_back(channel);
    }

    for(auto *ch : _channels) {
        auto *graph = _plot->addGraph();
        graph->setName(ch->name);

        _bindings.push_back({ ch, graph });
    }

    _plot->legend->setVisible(true);
    _plot->replot();

    _refreshTimer->start(33);
}

double ScopeWindow::extract(const Solver::Signal &sig, const ChannelLayout &ch) {
    switch(ch.kind) {
        case ChannelKind::Scalar:
            return std::get<Solver::Signal::Scalar>(sig.data);

        case ChannelKind::VectorElement:
            return std::get<Solver::Signal::Vector>(sig.data)[ch.index];

        case ChannelKind::BusField:
            return std::get<Solver::Signal::Bus>(sig.data).values()[ch.index];
    }

    return 0.0;
}
