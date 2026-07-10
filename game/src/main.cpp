#include <engine/window.hpp>
#include <engine/camera.hpp>
#include <engine/input.hpp>
#include <engine/mesh.hpp>
#include <engine/shader.hpp>
#include <engine/texture.hpp>
#include <engine/world.hpp>
#include <engine/jobs.hpp>

Window window(1280, 720, "VoxSim");

JobSystem jobs;

Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");

Texture stoneBrickTexture("res/textures/atlas.png");

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

World world;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void init() { Input::init(window.getHandle()); }

void update()
{
    Input::update();

    window.pollInputs();

    float thisFrame = (float)(glfwGetTime());
    deltaTime = thisFrame - lastFrame;
    lastFrame = thisFrame;

    if (Input::getKey(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window.getHandle(), true);

    if (Input::getKey(GLFW_KEY_W)) camera.move(Direction::Forward, deltaTime);
    if (Input::getKey(GLFW_KEY_S)) camera.move(Direction::Backward, deltaTime);
    if (Input::getKey(GLFW_KEY_A)) camera.move(Direction::Left, deltaTime);
    if (Input::getKey(GLFW_KEY_D)) camera.move(Direction::Right, deltaTime);
    if (Input::getKey(GLFW_KEY_LEFT_SHIFT)) camera.move(Direction::Down, deltaTime);
    if (Input::getKey(GLFW_KEY_SPACE)) camera.move(Direction::Up, deltaTime);

    double mouseX, mouseY;
    Input::getMouseDelta(mouseX, mouseY);

    camera.rotate(mouseX, mouseY, deltaTime);

    world.loadSphere(camera.position / 16.0f, 16);

    world.update(jobs);
}

void render()
{
    window.clear();

    shader.bind();

    shader.setUniform("projection", glm::perspective(glm::radians(45.0f), window.getAspect(), 0.01f, 1000.0f));
    shader.setUniform("view", camera.getViewMatrix());

    stoneBrickTexture.bind(0);

    world.draw(shader);

    window.swapBuffers();
}

int main()
{
    init();

    while (!window.shouldClose())
    {
        update();

        render();
    }

    return EXIT_SUCCESS;
}