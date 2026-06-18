#pragma once

#include <engine/chunk.hpp>
#include <engine/shader.hpp>
#include <engine/noise.hpp>
#include <engine/jobs.hpp>

#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>

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
    mutable std::mutex mapMutex;
    std::unordered_map<ChunkPosition, std::shared_ptr<Chunk>, ChunkHash> chunkTable;

    mutable std::mutex meshMutex;
    std::vector<std::unique_ptr<ChunkUpload>> meshQueue;

    std::vector<Chunk *> chunks;

    ChunkPosition previousOrigin{INT_MAX, INT_MAX, INT_MAX};
    int radius = 0;

public:
    World();

    void updateChunkSphere(const glm::vec3 &origin, int radius, JobSystem &jobs);

    void update(JobSystem &jobs);

    void draw(Shader &shader);

    void loadChunks(const std::vector<ChunkPosition> &positions, JobSystem &jobs);

    void unloadChunk(ChunkPosition position);

    bool chunkExists(ChunkPosition position) const;

    std::shared_ptr<Chunk> getChunk(ChunkPosition position) const;

    void queueMesh(std::unique_ptr<ChunkUpload> data);

    void generateChunk(const std::shared_ptr<Chunk> chunk);
};