#pragma once

#include <qschematic/items/item.hpp>

#include <functional>
#include <memory>

class QString;

namespace QSchematic::Items {
    class Item;
}

namespace gpds {
    class container;
}

namespace Windows {
    class BaseWindow;
}

class CustomItemFactory {
    public:
        CustomItemFactory(Windows::BaseWindow *window);

        std::shared_ptr<QSchematic::Items::Item> from_container(const gpds::container &container);

    private:
        CustomItemFactory(const CustomItemFactory &other) = default;
        CustomItemFactory(CustomItemFactory &&other) = default;

        Windows::BaseWindow *_window;
};