#pragma once

#include <engine/chunk.hpp>
#include <engine/shader.hpp>

#include <unordered_map>
#include <thread>
#include <mutex>
#include <atomic>
#include <vector>
#include <memory>
#include <cmath>

struct ChunkHash
{
    std::size_t operator()(const ChunkPosition &position) const noexcept
    {
        std::size_t hash = 0;

        hash ^= std::hash<int>{}(position.x) + 0x9E3779B9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>{}(position.y) + 0x9E3779B9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>{}(position.z) + 0x9E3779B9 + (hash << 6) + (hash >> 2);

        return hash;
    }
};

class World
{
private:
    std::thread workerThread;

    mutable std::mutex queueMutex;
    mutable std::mutex mapMutex;

    std::atomic<bool> isRunning{true};

    std::vector<ChunkPosition> genQueue;
    std::vector<ChunkPosition> dirtyQueue;

    std::vector<Chunk *> chunks;

    std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>, ChunkHash> chunkTable;

public:
    World();

    ~World();

    void workerLoop();

    Chunk *getChunkThreadSafe(ChunkPosition position);

    void update();

    void draw(Shader &shader);

    bool isLoaded(ChunkPosition position) const;

    bool chunkExists(ChunkPosition position) const;

    void updateRadius(glm::vec3 cameraPosition, int radius);

    void loadChunk(ChunkPosition position);

    void unloadChunk(ChunkPosition position);

    char getBlock(int x, int y, int z) const;

    void setBlock(int x, int y, int z, char type);

    void generateChunk(Chunk *chunk);

    void queueDirty(ChunkPosition position);

    const std::vector<Chunk *> &getChunks() const;
};