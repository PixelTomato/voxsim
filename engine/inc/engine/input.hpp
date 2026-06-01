#pragma once

#include <GLFW/glfw3.h>

class Input
{
    friend void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    friend void mouseCallback(GLFWwindow *window, double x, double y);

private:
    static bool keys[1024];

    static double lastX, lastY;
    static double deltaX, deltaY;

    static bool firstMouse;

public:
    static void init(GLFWwindow *window);

    static void update();

    static bool getKey(int key);

    static void getMouseDelta(double &x, double &y);
};