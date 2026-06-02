#include <engine/window.hpp>
#include <engine/camera.hpp>
#include <engine/input.hpp>
#include <engine/mesh.hpp>
#include <engine/shader.hpp>
#include <engine/texture.hpp>

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

    Input::init(window.getHandle());

    Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");

    Mesh quad(vertices, sizeof(vertices), indices, sizeof(indices));

    Texture stoneBrickTexture("res/textures/stone_bricks.png");

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!window.shouldClose())
    {
        Input::update();

        window.pollInputs();

        float thisFrame = (float)(glfwGetTime());
        deltaTime = thisFrame - lastFrame;
        lastFrame = thisFrame;

        if (Input::getKey(GLFW_KEY_ESCAPE))
            glfwSetWindowShouldClose(window.getHandle(), true);

        if (Input::getKey(GLFW_KEY_W))
            camera.move(0, deltaTime);
        if (Input::getKey(GLFW_KEY_S))
            camera.move(1, deltaTime);
        if (Input::getKey(GLFW_KEY_A))
            camera.move(2, deltaTime);
        if (Input::getKey(GLFW_KEY_D))
            camera.move(3, deltaTime);
        if (Input::getKey(GLFW_KEY_LEFT_SHIFT))
            camera.move(4, deltaTime);
        if (Input::getKey(GLFW_KEY_SPACE))
            camera.move(5, deltaTime);

        double mouseX, mouseY;
        Input::getMouseDelta(mouseX, mouseY);

        camera.rotate(mouseX, mouseY);

        window.clear();

        shader.bind();
        stoneBrickTexture.bind(0);

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), window.getAspect(), 0.01f, 1000.0f);
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = camera.getViewMatrix();

        shader.setUniform("projection", projection);
        shader.setUniform("model", model);
        shader.setUniform("view", view);

        quad.draw();

        window.swapBuffers();
    }

    return EXIT_SUCCESS;
}