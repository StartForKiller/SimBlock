#include <items/popup/popup.hpp>

Popup::Popup(QWidget *parent) :
    QWidget(parent)
{
    setFocusPolicy(Qt::NoFocus);

    QPalette p = palette();
    p.setColor(QPalette::Window, QColor("#F2F281"));
    setPalette(p);
}