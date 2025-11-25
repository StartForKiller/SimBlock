#pragma once

#include <qschematic/items/widget.hpp>

namespace Items::Widgets {

class TextEdit : public QSchematic::Items::Widget {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(TextEdit)

    public:
        explicit TextEdit(QGraphicsItem *parent = nullptr);

        ~TextEdit() override = default;
};

}