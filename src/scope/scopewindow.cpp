#include <scope/scopewindow.hpp>

#include <qcustomplot.h>

#include <QBoxLayout>

using namespace Scope;

ScopeWindow::ScopeWindow(const QString &title, QWidget *parent) :
    QMainWindow(parent)
{
    setAttribute(Qt::WA_DeleteOnClose, false);

    setWindowTitle(title);
    resize(800, 800);

    _plot = new QCustomPlot(this);
    _graph = _plot->addGraph();

    auto layout = new QVBoxLayout();
    layout->addWidget(_plot);

    auto window = new QWidget();
    window->setLayout(layout);

    setCentralWidget(window);
}

void ScopeWindow::onNewSample(double t, double value) {
    _graph->addData(t, value);
    _plot->xAxis->setRange(_plot->xAxis->range().lower, t + 0.5);
    _plot->replot(QCustomPlot::rpQueuedReplot);
}

void ScopeWindow::onStartSimulation() {
    _graph->data()->clear();
    _plot->replot(QCustomPlot::rpQueuedReplot);
}
