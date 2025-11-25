#pragma once

#include <items/popup/popup.hpp>
#include <items/operation.hpp>

#include <qschematic/items/label.hpp>

#include <QFormLayout>
#include <QLabel>

class PopupOperation : public Popup {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PopupOperation)

    public:
        explicit PopupOperation(const Operation &op) {
            auto layout = new QFormLayout;
            layout->addRow("Type:", new QLabel("Operation"));
            layout->addRow("Name:", new QLabel(op.label()->text()));
            setLayout(layout);
        }

        ~PopupOperation() noexcept override = default;
};