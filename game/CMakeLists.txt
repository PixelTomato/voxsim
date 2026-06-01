add_executable(voxsim
    src/main.cpp
)

target_include_directories(voxsim PRIVATE include)

target_link_libraries(voxsim PRIVATE engine)

set(ASSET_SRC "${CMAKE_SOURCE_DIR}/game/res")
set(ASSET_DST "${CMAKE_CURRENT_BINARY_DIR}/res")

add_custom_command(
    TARGET voxsim POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${ASSET_SRC}"
        "${ASSET_DST}"
    COMMENT "Copying assets to build directory..."
)