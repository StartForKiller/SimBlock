add_subdirectory(blocks)
add_subdirectory(popup)
add_subdirectory(widgets)

target_sources(
    ${target_library}
    PRIVATE
        customitemfactory.hpp
        fancywire.hpp
        itemtypes.hpp
)