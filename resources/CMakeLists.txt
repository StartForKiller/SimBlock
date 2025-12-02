target_sources(
    ${target_library}
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/icons/icons.qrc"
)

set_target_properties(
    ${target_library}
    PROPERTIES
        AUTORCC ON
)