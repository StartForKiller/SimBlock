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
        mainwindow.hpp
)