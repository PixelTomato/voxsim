#include <engine/window.hpp>

Window::Window(int width, int height, const char *title)
{
    if (!glfwInit())
    {
        std::cout << "Failed to intialize GLFW\n";
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    handle = glfwCreateWindow(width, height, "VoxSim", nullptr, nullptr);

    this->width = width;
    this->height = height;

    glfwMakeContextCurrent(handle);

    if (gladLoadGL((GLADloadfunc)glfwGetProcAddress) == 0)
    {
        std::cout << "Failed to initialize OpenGL with GLAD2\n";
    }

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);
    // glEnable(GL_CULL_FACE);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, width, height);
}

Window::~Window()
{
    glfwTerminate();
}

bool Window::shouldClose()
{
    return glfwWindowShouldClose(handle);
}

void Window::clear()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::swapBuffers()
{
    glfwSwapBuffers(handle);
}

void Window::pollInputs()
{
    glfwPollEvents();
}

GLFWwindow *Window::getHandle()
{
    return handle;
}

float Window::getWidth()
{
    return width;
}

float Window::getHeight()
{
    return height;
}

float Window::getAspect()
{
    return width / height;
}