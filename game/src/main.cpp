#include <engine/window.hpp>
#include <engine/camera.hpp>
#include <engine/input.hpp>
#include <engine/mesh.hpp>
#include <engine/shader.hpp>
#include <engine/texture.hpp>
#include <engine/world.hpp>

int main()
{
    Window window(1280, 720, "VoxSim");

    Input::init(window.getHandle());

    Shader shader("res/shaders/basic.vert", "res/shaders/basic.frag");

    Texture stoneBrickTexture("res/textures/sand.png");

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));

    World world;

    int worldWidth = 12;
    int worldHeight = 2;

    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    while (!window.shouldClose())
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

        glm::vec3 position = camera.getPosition();
        int cameraX = position.x / 16.0f;
        int cameraY = position.y / 16.0f;
        int cameraZ = position.z / 16.0f;

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

        window.clear();

        shader.bind();

        glm::mat4 projection = glm::perspective(glm::radians(45.0f), window.getAspect(), 0.01f, 1000.0f);

        glm::mat4 view = camera.getViewMatrix();

        shader.setUniform("projection", projection);
        shader.setUniform("view", view);

        stoneBrickTexture.bind(0);

        const auto &chunks = world.getChunks();
        for (const auto &chunk : chunks)
        {
            if (chunk->isReady())
            {
                ChunkPosition position = chunk->getPosition();

                glm::mat4 model = glm::mat4(1.0f);
                model = glm::translate(model, glm::vec3(position.x * 16.0f, position.y * 16.0f, position.z * 16.0f));
                shader.setUniform("model", model);

                chunk->getMesh()->draw();
            }
        }

        window.swapBuffers();
    }

    return EXIT_SUCCESS;
}