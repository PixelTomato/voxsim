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

Texture stoneBrickTexture("res/textures/stone_bricks.png");

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

World world;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void init()
{
    Input::init(window.getHandle());

    std::vector<ChunkPosition> chunks;

    int rad = 12;

    for (int x = -rad; x <= rad; x++)
        for (int y = -rad; y <= rad; y++)
            for (int z = -rad; z <= rad; z++)
                chunks.push_back({x, y, z});

    world.loadChunks(chunks, jobs);
}

void update()
{
    Input::update();

    window.pollInputs();

    float thisFrame = (float)(glfwGetTime());
    deltaTime = thisFrame - lastFrame;
    lastFrame = thisFrame;

    if (Input::getKey(GLFW_KEY_ESCAPE)) glfwSetWindowShouldClose(window.getHandle(), true);

    if (Input::getKey(GLFW_KEY_W)) camera.move(0, deltaTime);
    if (Input::getKey(GLFW_KEY_S)) camera.move(1, deltaTime);
    if (Input::getKey(GLFW_KEY_A)) camera.move(2, deltaTime);
    if (Input::getKey(GLFW_KEY_D)) camera.move(3, deltaTime);
    if (Input::getKey(GLFW_KEY_LEFT_SHIFT)) camera.move(4, deltaTime);
    if (Input::getKey(GLFW_KEY_SPACE)) camera.move(5, deltaTime);

    double mouseX, mouseY;
    Input::getMouseDelta(mouseX, mouseY);

    camera.rotate(mouseX, mouseY);

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