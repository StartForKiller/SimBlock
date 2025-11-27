add_subdirectory(blocks)
add_subdirectory(popup)
add_subdirectory(widgets)

target_sources(
    ${target}
    PRIVATE
        customitemfactory.cpp
        fancywire.cpp
)