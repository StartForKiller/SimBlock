#include <netlist/widget.hpp>
#include <netlist/viewer.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>

#include <qschematic/netlist_writer_json.hpp>

#include <QJsonDocument>
#include <QVBoxLayout>
#include <QPlainTextEdit>
#include <QTabWidget>

using namespace Netlist;
using namespace Blocks;

Widget::Widget(QWidget *parent) :
    QWidget(parent)
{
    m_memory_viewer = new Viewer;

    m_json_viewer = new QPlainTextEdit;
    m_json_viewer->setReadOnly(true);

    auto tab_widget = new QTabWidget;
    tab_widget->addTab(m_memory_viewer, tr("Memory"));
    tab_widget->addTab(m_json_viewer, tr("JSON"));

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(tab_widget);
    setLayout(layout);
}

void Widget::setNetlist(const QSchematic::Netlist<BaseBlock *, BaseBlockConnector *> &netlist) {
    m_memory_viewer->setNetlist(netlist);

    auto json = QSchematic::toJson(netlist);
    m_json_viewer->setPlainText(QJsonDocument(json).toJson(QJsonDocument::Indented));
}
