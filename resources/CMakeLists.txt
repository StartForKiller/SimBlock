target_sources(
    ${target}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/icons/icons.qrc"
)

set_target_properties(
    ${target}
    PROPERTIES
        AUTORCC ON
)