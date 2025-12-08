#pragma once

#include <qschematic/items/item.hpp>

enum ItemType {
    BaseBlockType = QSchematic::Items::Item::QSchematicItemUserType + 1,
    BaseBlockConnectorType,
    FancyWireType,
    BlockIntegratorType,
    BlockConstantType,
    BlockScopeType,
    BlockGainType,
    BlockSumType,
    BlockDivType,
    BlockTFType,
    BlockDerivatorType,
    BlockSubsystemType,
    BlockInputType,
    BlockOutputType,
    WidgetDial,
    WidgetTextedit
};