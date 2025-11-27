add_subdirectory(properties)

target_sources(
    ${target}
    PRIVATE
        baseblock.hpp
        baseblockconnector.hpp
        blockconstant.hpp
        blockdiv.hpp
        blockgain.hpp
        blockintegrator.hpp
        blockscope.hpp
        blocksum.hpp
)