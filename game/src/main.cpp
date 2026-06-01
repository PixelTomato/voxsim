#include <engine/texture.hpp>
#include <engine/window.hpp>
#include <engine/shader.hpp>
#include <engine/mesh.hpp>

float vertices[] = {
    -0.5f,
    -0.5f,
    0.0f,
    0.0f,
    0.0f,
    0.5f,
    -0.5f,
    0.0f,
    1.0f,
    0.0f,
    0.5f,
    0.5f,
    0.0f,
    1.0f,
    1.0f,
    -0.5f,
    0.5f,
    0.0f,
    0.0f,
    1.0f,
};

unsigned int indices[] = {
    0,
    1,
    2,
    2,
    3,
    0,
};

int main()
{
    Window window(1280, 720, "VoxSim");

    Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");

    Mesh quad(vertices, sizeof(vertices), indices, sizeof(indices));

    Texture stoneBrickTexture("res/textures/stone_bricks.png");

    while (!window.shouldClose())
    {
        window.clear();

        shader.bind();
        stoneBrickTexture.bind(0);

        quad.draw();

        window.swapBuffers();
        window.pollInputs();
    }

    return EXIT_SUCCESS;
}