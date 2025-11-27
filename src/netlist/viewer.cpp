#include <netlist/viewer.hpp>
#include <netlist/model.hpp>
#include <netlist/view.hpp>

#include <QBoxLayout>

using namespace Netlist;
using namespace Blocks;

Viewer::Viewer(QWidget *parent) :
    QWidget(parent)
{
    _model = new Model(this);

    _view = new View(this);
    _view->setModel(_model);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(_view);
    setLayout(layout);
}

void Viewer::setNetlist(const QSchematic::Netlist<BaseBlock *, BaseBlockConnector *> &netlist) {
    Q_ASSERT(_model);
    _model->setNetlist(netlist);
}
