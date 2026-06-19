#pragma once

#include <glad/gl.h>

#include <vector>

class Mesh
{
private:
    unsigned int VAO{0};
    unsigned int VBO{0};
    unsigned int EBO{0};

public:
    int vertexCount{0};
    int indexCount{0};

    Mesh();

    Mesh(const std::vector<float> &vertices, const std::vector<unsigned int> &indices);

    ~Mesh();

    Mesh(const Mesh &) = delete;
    Mesh &operator=(const Mesh &) = delete;

    Mesh(Mesh &&other) noexcept;
    Mesh &operator=(Mesh &&other) noexcept;

    void upload(const std::vector<float> &vertices, const std::vector<unsigned int> &indices);

    void draw() const;
};