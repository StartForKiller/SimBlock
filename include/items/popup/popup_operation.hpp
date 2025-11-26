#pragma once

#include <items/popup/popup.hpp>
#include <items/blocks/baseblock.hpp>

#include <qschematic/items/label.hpp>

#include <QFormLayout>
#include <QLabel>

using namespace Blocks;

class PopupOperation : public Popup {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PopupOperation)

    public:
        explicit PopupOperation(const BaseBlock &op) {
            auto layout = new QFormLayout;
            layout->addRow("Type:", new QLabel("BaseBlock"));
            layout->addRow("Name:", new QLabel(op.label()->text()));
            setLayout(layout);
        }

        ~PopupOperation() noexcept override = default;
};