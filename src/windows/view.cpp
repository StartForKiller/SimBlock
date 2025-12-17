#include <windows/view.hpp>
#include <items/blocks/baseblock.hpp>

#include <QWidget>
#include <QKeyEvent>

Windows::View::View(QWidget* parent) :
    QSchematic::View(parent)
{
}

void Windows::View::keyPressEvent(QKeyEvent* event) {
    if (event->modifiers() & Qt::ControlModifier) {
        switch (event->key()) {
            case Qt::Key_R:
                if (scene()) {
                    for(auto item : scene()->selectedItems()) {
                        auto blockPtr = dynamic_cast<Blocks::BaseBlock *>(item);
                        if(blockPtr == nullptr) continue;

                        blockPtr->rotateHandler();
                    }
                }
                return;
            default:
                break;
        }
    }

    switch (event->key()) {
        default:
            break;
    }

    QSchematic::View::keyPressEvent(event);
}