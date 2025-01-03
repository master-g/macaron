add_executable(samples
        olive.c
        stb_image_write.h
        main.c
)

set_target_properties(samples PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO
)

target_include_directories(samples PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(samples PUBLIC macaron box2d)

add_custom_command(
        TARGET samples POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/config/
        ${CMAKE_CURRENT_BINARY_DIR}/config/)
