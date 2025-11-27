#include <items/blocks/properties/blockpropertiesdialog.hpp>
#include <items/blocks/properties/blockproperty.hpp>
#include <items/blocks/baseblock.hpp>

#include <QVBoxLayout>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QLineEdit>
#include <QCheckBox>
#include <QPushButton>

using namespace Blocks;
using namespace Blocks::Properties;

PropertiesDialog::PropertiesDialog(BaseBlock *block, QWidget *parent) :
    QDialog(parent),
    _block(block)
{
    setWindowTitle(QStringLiteral("Block Properties: %1").arg(block->text()));

    auto layout = new QVBoxLayout(this);
    auto form = new QFormLayout;

    for(auto &prop : _block->properties()) {
        QWidget *editor = nullptr;
        QVariant v = prop.getter();

        switch(prop.type) {
            case BLOCK_PROPERTY_DOUBLE: {
                auto *spin = new QDoubleSpinBox;
                spin->setRange(prop.min, prop.max);
                spin->setValue(v.toDouble());
                spin->setDecimals(6);
                editor = spin;
                break;
            }

            case BLOCK_PROPERTY_INT: {
                auto *spin = new QSpinBox;
                spin->setRange((int)prop.min, (int)prop.max);
                spin->setValue(v.toInt());
                editor = spin;
                break;
            }

            case BLOCK_PROPERTY_BOOL: {
                auto *chk = new QCheckBox;
                chk->setChecked(v.toBool());
                editor = chk;
                break;
            }

            case BLOCK_PROPERTY_STRING: {
                auto *edt = new QLineEdit(v.toString());
                editor = edt;
                break;
            }

            default:
                continue;
        }

        _editors.push_back(editor);
        form->addRow(prop.name + ":", editor);
    }

    layout->addLayout(form);

    auto ok = new QPushButton("Accept");
    auto cancel = new QPushButton("Cancel");

    auto buttons = new QHBoxLayout;
    buttons->addStretch();
    buttons->addWidget(ok);
    buttons->addWidget(cancel);

    layout->addLayout(buttons);

    connect(ok, &QPushButton::clicked, this, &PropertiesDialog::onAccept);
    connect(cancel, &QPushButton::clicked, this, &PropertiesDialog::reject);
}

void PropertiesDialog::onAccept() {
    auto &props = _block->properties();

    for(int i = 0; i < props.size(); i++) {
        auto widget = _editors[i];
        auto prop_type = props[i].type;

        switch(prop_type) {
            case BLOCK_PROPERTY_DOUBLE:
                props[i].setter(static_cast<QDoubleSpinBox *>(widget)->value());
                break;

            case BLOCK_PROPERTY_INT:
                props[i].setter(static_cast<QSpinBox *>(widget)->value());
                break;

            case BLOCK_PROPERTY_BOOL:
                props[i].setter(static_cast<QCheckBox *>(widget)->isChecked());
                break;

            case BLOCK_PROPERTY_STRING:
                props[i].setter(static_cast<QLineEdit *>(widget)->text());
                break;
        }
    }

    accept();
}