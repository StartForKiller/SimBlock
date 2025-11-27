#pragma once

#include <items/popup/popup.hpp>
#include <items/blocks/baseblock.hpp>

#include <qschematic/items/label.hpp>

#include <QFormLayout>
#include <QLabel>

using namespace Blocks;

class PopupBaseBlock : public Popup {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PopupBaseBlock)

    public:
        explicit PopupBaseBlock(const BaseBlock &op) {
            auto layout = new QFormLayout;
            layout->addRow("Type:", new QLabel(QString(op.metaObject()->className()).split("::").last()));
            layout->addRow("Name:", new QLabel(op.label()->text()));
            setLayout(layout);
        }

        ~PopupBaseBlock() noexcept override = default;
};