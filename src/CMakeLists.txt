set(MACARON_SOURCE_FILES
        config_loader.c
        core.c
        core.h
        defaults.c
        game_state.c
        toml.c
        toml.h
)

set(MACARON_API_FILES
        ../include/macaron/base.h
        ../include/macaron/macaron.h
        ../include/macaron/types.h
)

# Hide internal functions
# todo need to investigate this more
# https://gcc.gnu.org/wiki/Visibility
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)

add_library(macaron ${MACARON_SOURCE_FILES} ${MACARON_API_FILES})

target_include_directories(macaron
        PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/../include>
        $<BUILD_INTERFACE:${CMAKE_CURRENT_BINARY_DIR}>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        ${CMAKE_CURRENT_SOURCE_DIR}/../extern/box2d/include
        PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}
)

source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}" PREFIX "src" FILES ${MACARON_SOURCE_FILES})
source_group(TREE "${CMAKE_CURRENT_SOURCE_DIR}/../include" PREFIX "include" FILES ${MACARON_API_FILES})

install(TARGETS macaron)
