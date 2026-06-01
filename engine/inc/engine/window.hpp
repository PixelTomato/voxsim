#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Window
{
    GLFWwindow *handle;

public:
    Window(int width, int height, const char *title);

    ~Window();

    bool shouldClose();

    void clear();

    void swapBuffers();

    void pollInputs();
};