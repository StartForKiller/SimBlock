#include <items/widgets/textedit.hpp>
#include <items/itemtypes.hpp>

#include <QTextEdit>

using namespace Items::Widgets;

TextEdit::TextEdit(QGraphicsItem *parent) : QSchematic::Items::Widget(::ItemType::WidgetTextedit, parent) {
    setWidget([] { return new QTextEdit; });
}