#include <items/widgets/dial.hpp>
#include <items/itemtypes.hpp>

#include <QDial>

using namespace Items::Widgets;

Dial::Dial(QGraphicsItem *parent) : QSchematic::Items::Widget(::ItemType::WidgetDial, parent) {
    setWidget([] { return new QDial; });
}