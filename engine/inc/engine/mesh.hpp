#pragma once

#include <glad/gl.h>

#include <span>

class Mesh
{
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    int indexCount;

public:
    Mesh(float *vertices, unsigned int vaSize, unsigned int *indices, unsigned int iaSize);

    ~Mesh();

    void draw();
};