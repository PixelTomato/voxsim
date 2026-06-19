#pragma once

#include <glm/glm.hpp>

#include <engine/mesh.hpp>

#include <memory>
#include <mutex>
#include <array>

template <typename T> class LockQueue
{
private:
    std::mutex mutex;
    std::vector<T> data;

public:
    void push(T value)
    {
        std::lock_guard lock(mutex);

        data.push_back(std::move(value));
    }

    std::vector<T> take()
    {
        std::lock_guard lock(mutex);

        std::vector<T> out;
        out.swap(data);

        return out;
    }
};

struct ChunkKey
{
    int x;
    int y;
    int z;

    bool operator==(const ChunkKey &other) const = default;
};

struct DataChunk
{
    ChunkKey key;

    std::array<char, 4096> blocks;
};

struct MeshChunk
{
    ChunkKey key;

    std::vector<float> vertices;
    std::vector<unsigned int> indices;
};

struct RenderChunk
{
    ChunkKey key;

    Mesh mesh;
};

struct ChunkPack
{
    std::shared_ptr<DataChunk> main;

    std::shared_ptr<DataChunk> neighbors[6];
};

class Chunk
{
public:
    inline static const int NEIGHBORS[6][3] = {
        {+0, +0, +1}, // front
        {+0, +0, -1}, // back
        {+1, +0, +0}, // right
        {-1, +0, +0}, // left
        {+0, +1, +0}, // top
        {+0, -1, +0}, // bottom
    };

    static void buildMesh(ChunkKey key, ChunkPack chunkPack, MeshChunk &mesh);

    inline static int getIndex(int x, int y, int z) { return (x << 8) | (y << 4) | z; }
};