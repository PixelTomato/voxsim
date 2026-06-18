#pragma once

#include <glm/glm.hpp>

#include <engine/mesh.hpp>

#include <memory>
#include <atomic>
#include <array>

class World;

enum class ChunkState : char
{
    AwaitingData,
    Generation,
    AwaitingMesh,
    Meshing,
    Ready,
};

struct ChunkPosition
{
    int x;
    int y;
    int z;

    bool operator==(const ChunkPosition &other) const = default;

    glm::vec3 toVec3() const { return glm::vec3(x, y, z); }
};

struct ChunkUpload
{
    ChunkPosition position;

    std::vector<float> vertices;

    std::vector<unsigned int> indices;

    ChunkUpload(ChunkPosition position) : position(position) {}
};

class Chunk
{
private:
    std::array<char, 4096> blocks{};

    ChunkPosition position;

    std::unique_ptr<Mesh> mesh = nullptr;

public:
    std::atomic<ChunkState> state{ChunkState::AwaitingData};

    std::atomic<int> awaitingData{7};

    std::shared_ptr<Chunk> neighbors[6] = {};

    inline static const int NEIGHBORS[6][3] = {
        {+0, +0, +1}, // front
        {+0, +0, -1}, // back
        {+1, +0, +0}, // right
        {-1, +0, +0}, // left
        {+0, +1, +0}, // top
        {+0, -1, +0}, // bottom
    };

    std::size_t index{0};

    Chunk(ChunkPosition position) : position(position) {}

    void setBlock(int x, int y, int z, char type) { blocks[getIndex(x, y, z)] = type; }

    char getBlock(int x, int y, int z) const { return blocks[getIndex(x, y, z)]; }

    ChunkPosition getPosition() const { return position; }

    Mesh *getMesh() const { return mesh.get(); }

    void setMesh(std::unique_ptr<Mesh> mesh) { this->mesh = std::move(mesh); }

    std::unique_ptr<ChunkUpload> buildMesh();

private:
    inline int getIndex(int x, int y, int z) const { return (x << 8) | (y << 4) | z; }
};