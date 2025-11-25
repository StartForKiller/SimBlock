cmake_minimum_required(VERSION 3.12)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(target SimBlock)
project(
    ${target}
    VERSION 0.0.1
    LANGUAGES C CXX
)

find_package(
    Qt6
    COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        Svg
)

include(FetchContent)

set(QSCHEMATIC_BUILD_DEMO               OFF          CACHE INTERNAL "")
set(QSCHEMATIC_DEPENDENCY_GPDS_DOWNLOAD ON          CACHE INTERNAL "")
set(QSCHEMATIC_DEPENDENCY_GPDS_TARGET  "gpds-shared" CACHE INTERNAL "")

FetchContent_Declare(
    qschematic
    GIT_REPOSITORY https://github.com/simulton/qschematic
    GIT_TAG        3.0.3
)
FetchContent_MakeAvailable(qschematic)

FetchContent_Declare(
  qcustomplot
  GIT_REPOSITORY https://github.com/avm-energo/qcustomplot.git
  GIT_TAG        origin/develop
)
FetchContent_MakeAvailable(qcustomplot)

add_executable(${target})

add_subdirectory(resources)

target_compile_features(
    ${target}
    PUBLIC
        cxx_std_23
)

target_sources(
    ${target}
    PRIVATE
    FILE_SET
        HEADERS
        FILES
            include/commands/node_add_connector.hpp
            include/commands/commands.hpp
            include/common/treeitem.hpp
            include/common/treemodel.hpp
            include/items/popup/popup.hpp
            include/items/popup/popup_connector.hpp
            include/items/popup/popup_operation.hpp
            include/items/widgets/dial.hpp
            include/items/widgets/textedit.hpp
            include/items/customitemfactory.hpp
            include/items/fancywire.hpp
            include/items/itemtypes.hpp
            include/items/operation.hpp
            include/items/operationconnector.hpp
            include/items/operationconstant.hpp
            include/items/operationdemo1.hpp
            include/items/operationscope.hpp
            include/library/iteminfo.hpp
            include/library/iteminfo.hpp
            include/library/modelitem.hpp
            include/library/model.hpp
            include/library/view.hpp
            include/library/widget.hpp
            include/netlist/model.hpp
            include/netlist/view.hpp
            include/netlist/viewer.hpp
            include/netlist/widget.hpp
            include/scope/scopewindow.hpp
            include/solver/simulationworker.hpp
            include/solver/solver.hpp
            include/mainwindow.hpp

    PRIVATE
        src/commands/node_add_connector.cpp
        src/common/treeitem.cpp
        src/common/treemodel.cpp
        src/items/popup/popup.cpp
        src/items/widgets/dial.cpp
        src/items/widgets/textedit.cpp
        src/items/customitemfactory.cpp
        src/items/fancywire.cpp
        src/items/operation.cpp
        src/items/operationconnector.cpp
        src/items/operationconstant.cpp
        src/items/operationdemo1.cpp
        src/items/operationscope.cpp
        src/library/model.cpp
        src/library/view.cpp
        src/library/widget.cpp
        src/netlist/model.cpp
        src/netlist/view.cpp
        src/netlist/viewer.cpp
        src/netlist/widget.cpp
        src/scope/scopewindow.cpp
        src/solver/simulationworker.cpp
        src/solver/solver.cpp
        src/main.cpp
        src/mainwindow.cpp
)

target_include_directories(
    ${target}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)

target_link_libraries(
    ${target}
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
    ${target}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
)

set_target_properties(
    ${target}
    PROPERTIES
        OUTPUT_NAME "SimBlock"
        ARCHIVE_OUTPUT_NAME "SimBlock"
        VERSION "v0.0.1"
        POSITION_INDEPENDENT_CODE ON
)

if (WINDOW_WIDTH)
    add_compile_definitions(WINDOW_WIDTH=${WINDOW_WIDTH})
endif()
if (WINDOW_HEIGHT)
    add_compile_definitions(WINDOW_HEIGHT=${WINDOW_HEIGHT})
endif()
# Usage: Add -DWINDOW_MAXIMIZED=ON to the cmake options
if (WINDOW_MAXIMIZED)
    add_compile_definitions(WINDOW_MAXIMIZE)
endif()