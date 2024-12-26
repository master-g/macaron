cmake_minimum_required(VERSION 3.22)
include(FetchContent)

project(macaron
    VERSION 0.0.1
    DESCRIPTION "A solver for game carrom using Box2D"
    LANGUAGES C CXX
)

# stuff to help debug cmake
message(STATUS "CMake C compiler: ${CMAKE_C_COMPILER_ID}")
message(STATUS "CMake C++ compiler: ${CMAKE_CXX_COMPILER_ID}")
message(STATUS "CMake system name: ${CMAKE_SYSTEM_NAME}")
message(STATUS "CMake host system processor: ${CMAKE_HOST_SYSTEM_PROCESSOR}")

if(PROJECT_IS_TOP_LEVEL)
    # Needed for samples.exe to find macaron.dll
    # set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}/bin")
endif()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_subdirectory(extern/box2d)
add_subdirectory(src)

if(PROJECT_IS_TOP_LEVEL)
    option(MACARON_SAMPLES "Build the Macaron samples" ON)

    # enkiTS is used by all the test apps, but not directly by the Macaron library
    if(MACARON_SAMPLES)
        SET(ENKITS_BUILD_EXAMPLES OFF CACHE BOOL "Build enkiTS examples")

        # Used in tests and samples
        FetchContent_Declare(
            enkits
            GIT_REPOSITORY https://github.com/dougbinks/enkiTS.git
            GIT_TAG master
            GIT_SHALLOW TRUE
            GIT_PROGRESS TRUE
        )
        FetchContent_MakeAvailable(enkits)
    endif()

    if(MACARON_SAMPLES)
        add_subdirectory(samples)

        # default startup project for Visual Studio
        if(MSVC)
            set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT samples)
            set_property(TARGET samples PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
        endif()
    endif()
endif()