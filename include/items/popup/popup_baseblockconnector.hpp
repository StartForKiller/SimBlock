#pragma once

#include <items/popup/popup.hpp>
#include <items/blocks/baseblockconnector.hpp>

#include <qschematic/items/label.hpp>

#include <QFormLayout>
#include <QLabel>

class PopupBaseBlockConnector : public Popup {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PopupBaseBlockConnector)

    public:
        explicit PopupBaseBlockConnector(const Blocks::BaseBlockConnector &conn) {
            auto layout = new QFormLayout;
            layout->addRow("Type:", new QLabel("Connector"));
            layout->addRow("Name:", new QLabel(conn.label()->text()));
            setLayout(layout);
        }

        ~PopupBaseBlockConnector() noexcept override = default;
};