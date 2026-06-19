#pragma once

#include <engine/chunk.hpp>
#include <engine/shader.hpp>
#include <engine/noise.hpp>
#include <engine/jobs.hpp>

#include <unordered_map>
#include <unordered_set>

struct ChunkHash
{
    std::size_t operator()(const ChunkKey &key) const noexcept
    {
        std::size_t hash = 0;

        hash ^= std::hash<int>{}(key.x) + 0x9E3779B9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>{}(key.y) + 0x9E3779B9 + (hash << 6) + (hash >> 2);
        hash ^= std::hash<int>{}(key.z) + 0x9E3779B9 + (hash << 6) + (hash >> 2);

        return hash;
    }
};

class World
{
private:
    std::unordered_set<ChunkKey, ChunkHash> loadedKeys;
    std::unordered_set<ChunkKey, ChunkHash> genStage;
    std::unordered_set<ChunkKey, ChunkHash> meshStage;

    std::unordered_map<ChunkKey, std::shared_ptr<DataChunk>, ChunkHash> dataChunks;
    std::unordered_map<ChunkKey, RenderChunk, ChunkHash> renderChunks;

    LockQueue<std::shared_ptr<DataChunk>> genJobOutput;
    LockQueue<MeshChunk> meshJobOutput;

public:
    World() { Noise(); }

    void update(JobSystem &jobs);

    void draw(Shader &shader);

    void loadSphere(glm::vec3 origin, int radius);

private:
    void collectData();

    void collectMeshes();

    void scheduleGen(JobSystem &jobs);

    bool canMesh(ChunkKey key) const;

    void scheduleMeshes(JobSystem &jobs);

    void unloadChunks();

    void generateChunk(std::shared_ptr<DataChunk> chunk);
};