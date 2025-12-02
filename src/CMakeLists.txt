add_subdirectory(commands)
add_subdirectory(common)
add_subdirectory(items)
add_subdirectory(library)
add_subdirectory(netlist)
add_subdirectory(scope)
add_subdirectory(solver)

target_sources(
    ${target_library}
    PRIVATE
        mainwindow.cpp
)

target_compile_features(
    ${target_library}
    PUBLIC
        cxx_std_23
)

target_include_directories(
    ${target_library}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
)

target_link_libraries(
    ${target_library}
    PRIVATE
        Qt::Core
        Qt::Gui
        Qt::Widgets
        Qt::PrintSupport
        Qt::Svg
        qschematic::qschematic-static
        qcustomplot
)

set_target_properties(
    ${target_library}
    PROPERTIES
        POSITION_INDEPENDENT_CODE ON
        AUTOMOC ON
        AUTORCC ON
)

target_sources(
    ${target}
    PRIVATE
        main.cpp
)