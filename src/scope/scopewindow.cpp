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
}

void ScopeWindow::onNewSample(double t, Solver::Signal value) {
    if(Solver::isScalar(value)) {
        _graph->addData(t, std::get<Solver::Signal::Scalar>(value.data));
        _plot->xAxis->setRange(_plot->xAxis->range().lower, t + 0.5);
        _plot->replot(QCustomPlot::rpQueuedReplot);
    }
}

void ScopeWindow::onStartSimulation() {
    _graph->data()->clear();
    _plot->replot(QCustomPlot::rpQueuedReplot);
}
