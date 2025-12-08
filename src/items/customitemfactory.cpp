#include <items/itemtypes.hpp>
#include <items/blocks/baseblock.hpp>
#include <items/blocks/baseblockconnector.hpp>

#include <items/blocks/blockconstant.hpp>
#include <items/blocks/blockintegrator.hpp>
#include <items/blocks/blockscope.hpp>
#include <items/blocks/blockgain.hpp>
#include <items/blocks/blocksum.hpp>
#include <items/blocks/blockdiv.hpp>
#include <items/blocks/blocktf.hpp>
#include <items/blocks/blockderivator.hpp>
#include <items/blocks/blocksubsystem.hpp>
#include <items/blocks/blockinput.hpp>
#include <items/blocks/blockoutput.hpp>

#include <items/customitemfactory.hpp>
#include <items/fancywire.hpp>
#include <items/widgets/dial.hpp>
#include <items/widgets/textedit.hpp>

#include <qschematic/items/itemfactory.hpp>

using namespace Blocks;

CustomItemFactory::CustomItemFactory(Windows::BaseWindow *window) :
    _window(window)
{

}

std::shared_ptr<QSchematic::Items::Item> CustomItemFactory::from_container(const gpds::container &container) {
    QSchematic::Items::Item::ItemType type = QSchematic::Items::Factory::extractType(container);

    switch(static_cast<ItemType>(type)) {
        case ItemType::BaseBlockType:
            return std::make_shared<BaseBlock>(::ItemType::BaseBlockType, _window);

        case ItemType::BaseBlockConnectorType:
            return std::make_shared<BaseBlockConnector>();

        case ItemType::BlockIntegratorType:
            return std::make_shared<BlockIntegrator>(_window);

        case ItemType::BlockConstantType:
            return std::make_shared<BlockConstant>(_window);

        case ItemType::BlockScopeType:
            return std::make_shared<BlockScope>(_window);

        case ItemType::BlockGainType:
            return std::make_shared<BlockGain>(_window);

        case ItemType::BlockSumType:
            return std::make_shared<BlockSum>(_window);

        case ItemType::BlockDivType:
            return std::make_shared<BlockDiv>(_window);

        case ItemType::BlockTFType:
            return std::make_shared<BlockTF>(_window);

        case ItemType::BlockDerivatorType:
            return std::make_shared<BlockDerivator>(_window);

        case ItemType::BlockSubsystemType:
            return std::make_shared<BlockSubsystem>(_window);

        case ItemType::BlockInputType:
            return std::make_shared<BlockInput>(_window);

        case ItemType::BlockOutputType:
            return std::make_shared<BlockOutput>(_window);

        case ItemType::FancyWireType:
            return std::make_shared<FancyWire>();

        case ItemType::WidgetDial:
            return std::make_shared<Items::Widgets::Dial>();

        case ItemType::WidgetTextedit:
            return std::make_shared<Items::Widgets::TextEdit>();
    }

    return {};
}