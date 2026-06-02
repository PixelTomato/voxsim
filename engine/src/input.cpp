#include <engine/input.hpp>

bool Input::keys[1024] = {false};

double Input::lastX = 0.0;
double Input::lastY = 0.0;

double Input::deltaX = 0.0;
double Input::deltaY = 0.0;

bool Input::firstMouse = true;

void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (key >= 0 && key < 1024)
    {
        if (action == GLFW_PRESS)
        {
            Input::keys[key] = true;
        }

        if (action == GLFW_RELEASE)
        {
            Input::keys[key] = false;
        }
    }
}

void mouseCallback(GLFWwindow *window, double x, double y)
{
    if (Input::firstMouse)
    {
        Input::lastX = x;
        Input::lastY = y;

        Input::firstMouse = false;
    }

    Input::deltaX = x - Input::lastX;
    Input::deltaY = Input::lastY - y;

    Input::lastX = x;
    Input::lastY = y;
}

void Input::init(GLFWwindow *window)
{
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    Input::lastX = width / 2.0f;
    Input::lastY = height / 2.0f;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (glfwRawMouseMotionSupported())
    {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    glfwSetKeyCallback(window, keyCallback);
    glfwSetCursorPosCallback(window, mouseCallback);
}

void Input::update()
{
    Input::deltaX = 0.0;
    Input::deltaY = 0.0;
}

bool Input::getKey(int key)
{
    if (key >= 0 && key < 1024)
    {
        return Input::keys[key];
    }

    return false;
}

void Input::getMouseDelta(double &x, double &y)
{
    x = Input::deltaX;
    y = Input::deltaY;
}