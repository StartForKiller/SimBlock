#pragma once

#include <QDialog>

namespace Blocks {
    class BaseBlock;
};

namespace Blocks::Properties {

class PropertiesDialog : public QDialog {
    Q_OBJECT
    Q_DISABLE_COPY_MOVE(PropertiesDialog)

    public:
        explicit PropertiesDialog(BaseBlock *block, QWidget *parent = nullptr);
        ~PropertiesDialog() override = default;

    private slots:
        void onAccept();

    private:
        BaseBlock *_block;
        QVector<QWidget *> _editors;
};

}