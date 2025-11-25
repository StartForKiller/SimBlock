#include <items/itemtypes.hpp>
#include <items/operation.hpp>
#include <items/operationconnector.hpp>
#include <items/operationconstant.hpp>
#include <items/operationdemo1.hpp>
#include <items/operationscope.hpp>
#include <items/customitemfactory.hpp>
#include <items/fancywire.hpp>
#include <items/widgets/dial.hpp>
#include <items/widgets/textedit.hpp>

#include <qschematic/items/itemfactory.hpp>

std::shared_ptr<QSchematic::Items::Item> CustomItemFactory::from_container(const gpds::container &container) {
    QSchematic::Items::Item::ItemType type = QSchematic::Items::Factory::extractType(container);

    switch(static_cast<ItemType>(type)) {
        case ItemType::OperationType:
            return std::make_shared<Operation>();

        case ItemType::OperationConnectorType:
            return std::make_shared<OperationConnector>();

        case ItemType::OperationDemo1Type:
            return std::make_shared<OperationDemo1>();

        case ItemType::OperationConstantType:
            return std::make_shared<OperationConstant>();

        case ItemType::OperationScopeType:
            return std::make_shared<OperationScope>();

        case ItemType::FancyWireType:
            return std::make_shared<FancyWire>();

        case ItemType::WidgetDial:
            return std::make_shared<Items::Widgets::Dial>();

        case ItemType::WidgetTextedit:
            return std::make_shared<Items::Widgets::TextEdit>();
    }

    return {};
}