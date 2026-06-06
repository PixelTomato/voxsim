#include <engine/window.hpp>
#include <engine/camera.hpp>
#include <engine/input.hpp>
#include <engine/mesh.hpp>
#include <engine/shader.hpp>
#include <engine/texture.hpp>
#include <engine/world.hpp>

Window window(1280, 720, "VoxSim");

Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");

Texture stoneBrickTexture("res/textures/bricks.png");

Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

World world;

int worldWidth = 12;
int worldHeight = 2;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

void init()
{
    Input::init(window.getHandle());
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

    glm::vec3 position = camera.getPosition() / 16.0f;
    int cameraX = position.x;
    int cameraY = position.y;
    int cameraZ = position.z;

    for (int x = cameraX - worldWidth; x <= cameraX + worldWidth; x++)
    {
        for (int y = 0; y <= worldHeight; y++)
        {
            for (int z = cameraZ - worldWidth; z <= cameraZ + worldWidth; z++)
            {
                int xDist = cameraX - x;
                int zDist = cameraZ - z;

                if ((xDist * xDist + zDist * zDist) < 12 * 12) world.loadChunk({x, y, z});
            }
        }
    }
}

void render()
{
    window.clear();

    shader.bind();

    shader.setUniform("projection", glm::perspective(glm::radians(45.0f), window.getAspect(), 0.01f, 1000.0f));
    shader.setUniform("view", camera.getViewMatrix());

    stoneBrickTexture.bind(0);

    const auto &chunks = world.getChunks();
    for (const auto &chunk : chunks)
    {
        if (chunk->isReady())
        {
            shader.setUniform("model", glm::translate(glm::mat4(1.0f), chunk->getPosition().toVec3() * 16.0f));

            chunk->getMesh()->draw();
        }
    }

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