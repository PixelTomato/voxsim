#pragma once

#include <glm/glm.hpp>

#include <engine/mesh.hpp>

#include <memory>
#include <array>

struct ChunkPosition
{
    int x;
    int y;
    int z;

    bool operator==(const ChunkPosition &other) const
    {
        return (x == other.x) && (y == other.y) && (z == other.z);
    }

    glm::vec3 toVec3() const
    {
        return glm::vec3(x, y, z);
    }
};

class Chunk
{
private:
    std::array<char, 4096> blocks;

    std::unique_ptr<Mesh> mesh = nullptr;

    ChunkPosition position;

    bool ready = false;

public:
    Chunk(ChunkPosition position);

    void setBlock(int x, int y, int z, char type);

    char getBlock(int x, int y, int z) const;

    bool isReady() const;

    void setReady(bool state);

    void rebuildMesh();

    const Mesh *getMesh() const;

    ChunkPosition getPosition() const;

private:
    inline int getIndex(int x, int y, int z) const;
};