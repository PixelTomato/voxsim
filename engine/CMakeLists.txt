add_library(engine STATIC
    src/camera.cpp
    src/input.cpp
    src/mesh.cpp
    src/shader.cpp
    src/texture.cpp
    src/window.cpp
)

target_include_directories(engine PUBLIC inc)

target_link_libraries(engine 
    PUBLIC 
        glfw 
        glad
        stb
        glm::glm
)