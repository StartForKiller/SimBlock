set(TEST_SOURCES
    test_mainwindow.cpp
)

foreach(test_src ${TEST_SOURCES})
    get_filename_component(test_name ${test_src} NAME_WE)

    add_executable(${test_name})

    target_include_directories(
        ${test_name}
        PRIVATE
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include/>
    )

    target_sources(
        ${test_name}
        PRIVATE
            ${test_src}
    )

    target_link_libraries(
        ${test_name}
        PRIVATE
            Qt::Test
            Qt::Widgets
            qschematic::qschematic-static
            ${target_library}
    )

    set_target_properties(
        ${test_name}
        PROPERTIES
            POSITION_INDEPENDENT_CODE ON
            AUTOMOC ON
            AUTORCC ON
    )

    add_test(NAME ${test_name} COMMAND ${test_name})
endforeach()