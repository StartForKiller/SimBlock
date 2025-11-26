#pragma once

#include <qschematic/items/item.hpp>

enum ItemType {
    BaseBlockType = QSchematic::Items::Item::QSchematicItemUserType + 1,
    OperationConnectorType,
    FancyWireType,
    OperationDemo1Type,
    OperationConstantType,
    OperationScopeType,
    WidgetDial,
    WidgetTextedit
};