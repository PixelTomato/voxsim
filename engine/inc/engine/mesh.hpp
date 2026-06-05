#pragma once

#include <glad/gl.h>

#include <vector>

class Mesh
{
private:
    unsigned int VAO;
    unsigned int VBO;
    unsigned int EBO;

    int indexCount = 0;

public:
    Mesh();

    Mesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices);

    ~Mesh();

    void upload(const std::vector<float> &vertices, const std::vector<unsigned int> &indices);

    void draw() const;
};