#pragma once

#include <engine/chunk.hpp>

#include <unordered_map>
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
    std::vector<ChunkPosition> genQueue;

    std::vector<Chunk *> chunks;

    std::unordered_map<ChunkPosition, std::unique_ptr<Chunk>, ChunkHash> chunkTable;

public:
    void update();

    bool isLoaded(ChunkPosition position) const;

    bool chunkExists(ChunkPosition position) const;

    void loadChunk(ChunkPosition position);

    void unloadChunk(ChunkPosition position);

    char getBlock(int x, int y, int z) const;

    void setBlock(int x, int y, int z, char type);

    void generateChunk(Chunk *chunk);

    const std::vector<Chunk *> &getChunks() const;
};