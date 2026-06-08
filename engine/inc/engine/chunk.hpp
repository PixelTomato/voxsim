#pragma once

#include <glm/glm.hpp>

#include <engine/mesh.hpp>

#include <memory>
#include <array>

class World;

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

    bool generated = false;
    bool meshed = false;

    bool dirty = false;

public:
    Chunk *neighbors[6] = {
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
        nullptr,
    };

    inline static const int NEIGHBORS[6][3] = {
        {+0, +0, +1}, // front
        {+0, +0, -1}, // back
        {+1, +0, +0}, // right
        {-1, +0, +0}, // left
        {+0, +1, +0}, // top
        {+0, -1, +0}, // bottom
    };

    std::size_t index;

    Chunk(ChunkPosition position);

    void setBlock(int x, int y, int z, char type);

    char getBlock(int x, int y, int z) const;

    bool isMeshed() const;

    void setMeshed(bool state);

    bool isGenerated() const;

    void setGenerated(bool state);

    void rebuildMesh();

    void markDirty(World &world);

    void markClean();

    bool isDirty() const;

    const Mesh *getMesh() const;

    ChunkPosition getPosition() const;

private:
    inline int getIndex(int x, int y, int z) const;
};