add_subdirectory(properties)

target_sources(
    ${target_library}
    PRIVATE
        baseblock.hpp
        baseblockconnector.hpp
        blockconstant.hpp
        blockderivator.hpp
        blockdiv.hpp
        blockgain.hpp
        blockintegrator.hpp
        blockscope.hpp
        blocksum.hpp
        blocktf.hpp
)