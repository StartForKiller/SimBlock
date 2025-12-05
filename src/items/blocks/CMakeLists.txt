add_subdirectory(properties)

target_sources(
    ${target_library}
    PRIVATE
        baseblock.cpp
        baseblockconnector.cpp
        blockconstant.cpp
        blockderivator.cpp
        blockdiv.cpp
        blockgain.cpp
        blockintegrator.cpp
        blockscope.cpp
        blocksubsystem.cpp
        blocksum.cpp
        blocktf.cpp
)