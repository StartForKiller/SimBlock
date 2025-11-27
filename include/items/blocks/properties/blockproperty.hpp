#pragma once

#include <QString>
#include <QVariant>

namespace Blocks::Properties {

enum BlockPropertyType {
    BLOCK_PROPERTY_STRING,
    BLOCK_PROPERTY_DOUBLE,
    BLOCK_PROPERTY_INT,
    BLOCK_PROPERTY_BOOL
};

struct BlockProperty {
    QString name;
    BlockPropertyType type;
    double min = -1e9;
    double max = 1e9;
    std::function<void(const QVariant&)> setter;
    std::function<QVariant()> getter;
};

}