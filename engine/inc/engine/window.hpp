#pragma once

#include <glad/gl.h>
#include <GLFW/glfw3.h>

#include <iostream>

class Window
{
    GLFWwindow *handle;

    float width;
    float height;

public:
    Window(int width, int height, const char *title);

    ~Window();

    bool shouldClose();

    void clear();

    void swapBuffers();

    void pollInputs();

    GLFWwindow *getHandle();

    float getWidth();

    float getHeight();

    float getAspect();
};