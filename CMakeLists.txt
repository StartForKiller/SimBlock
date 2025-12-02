cmake_minimum_required(VERSION 3.21)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

set(target SimBlock)
set(target_library SimBlockLib)
project(
    ${target}
    VERSION 0.0.1
    LANGUAGES C CXX
)

find_package(
    Qt6
    REQUIRED
    COMPONENTS
        Core
        Gui
        Widgets
        PrintSupport
        Svg
        Test
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
add_library(${target_library})

add_subdirectory(resources)

target_compile_features(
    ${target}
    PUBLIC
        cxx_std_23
)

add_subdirectory(include)
add_subdirectory(src)

enable_testing()
add_subdirectory(tests)

target_include_directories(
    ${target}
    PRIVATE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include/>
)

target_link_libraries(
    ${target}
    PRIVATE
        Qt::Core
        Qt::Widgets
        qschematic::qschematic-static
        ${target_library}
)

set_target_properties(
    ${target}
    PROPERTIES
        OUTPUT_NAME "SimBlock"
        ARCHIVE_OUTPUT_NAME "SimBlock"
        VERSION "v0.0.1"
        POSITION_INDEPENDENT_CODE ON
        AUTOMOC ON
        AUTORCC ON
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