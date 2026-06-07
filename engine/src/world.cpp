#include <engine/world.hpp>

#include <iostream>

void World::update()
{
    for (int i = 0; i < 5; i++)
    {
        if (!genQueue.empty())
        {
            ChunkPosition position = genQueue.back();

            genQueue.pop_back();

            if (!isLoaded(position)) continue;

            Chunk *chunk = chunkTable[position].get();

            generateChunk(chunk);

            for (int i = 0; i < 6; i++)
            {
                Chunk *neighbor = chunk->neighbors[i];

                if (neighbor != nullptr && neighbor->isReady())
                {
                    neighbor->markDirty(*this);
                }
            }
        }
    }

    for (int i = 0; i < 5; i++)
    {
        if (!dirtyQueue.empty())
        {
            ChunkPosition position = dirtyQueue.back();

            dirtyQueue.pop_back();

            if (!isLoaded(position)) continue;

            Chunk *chunk = chunkTable[position].get();

            generateChunk(chunk);
        }
    }
}

bool World::isLoaded(ChunkPosition position) const
{
    return chunkTable.contains(position);
}

bool World::chunkExists(ChunkPosition position) const
{
    return false;
}

void World::loadChunk(ChunkPosition position)
{
    if (isLoaded(position)) return;

    std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>(position);

    chunk->index = chunks.size();
    chunks.push_back(chunk.get());

    for (int i = 0; i < 6; i++)
    {
        ChunkPosition offset = {position.x + Chunk::NEIGHBORS[i][0],
                                position.y + Chunk::NEIGHBORS[i][1],
                                position.z + Chunk::NEIGHBORS[i][2]};

        auto target = chunkTable.find(offset);
        if (target != chunkTable.end())
        {
            Chunk *neighbor = target->second.get();

            chunk->neighbors[i] = neighbor;
            neighbor->neighbors[i ^ 1] = chunk.get();
        }
    }

    chunkTable[position] = std::move(chunk);

    genQueue.push_back(position);
}

void World::unloadChunk(ChunkPosition position)
{
    auto target = chunkTable.find(position);

    if (target == chunkTable.end()) return;

    Chunk *chunk = target->second.get();

    for (int i = 0; i < 6; i++)
    {
        Chunk *neighbor = chunk->neighbors[i];

        if (neighbor != nullptr)
        {
            neighbor->neighbors[i ^ 1] = nullptr;
        }
    }

    std::size_t index = chunk->index;

    if (index < chunks.size() - 1)
    {
        chunks[index] = chunks.back();

        chunks[index]->index = index;
    }

    chunks.pop_back();

    chunkTable.erase(position);
}

char World::getBlock(int x, int y, int z) const
{
    auto index = chunkTable.find({x >> 4, y >> 4, z >> 4});

    if (index == chunkTable.end())
    {
        return 0;
    }

    return index->second->getBlock(x & 15, y & 15, z & 15);
}

void World::setBlock(int x, int y, int z, char type)
{
    auto index = chunkTable.find({x >> 4, y >> 4, z >> 4});

    if (index == chunkTable.end())
    {
        return;
    }

    index->second->setBlock(x & 15, y & 15, z & 15, type);
}

void World::generateChunk(Chunk *chunk)
{
    ChunkPosition position = chunk->getPosition();

    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int z = 0; z < 16; z++)
            {
                int wx = (float)(position.x * 16 + x);
                int wy = (float)(position.y * 16 + y);
                int wz = (float)(position.z * 16 + z);

                float hills = std::sin(wx * 0.08f) * std::cos(wz * 0.08f) * 6.0f;
                float bumps = std::sin(wx * 0.25f) * std::sin(wz * 0.15f) * 3.0f;

                char blockType = 0;

                if (wy < (hills + bumps + 10.0f))
                {
                    blockType = 1;
                }

                chunk->setBlock(x, y, z, blockType);
            }
        }
    }

    chunk->rebuildMesh();

    chunk->setReady(true);

    chunk->markClean();
}

void World::queueDirty(ChunkPosition position)
{
    dirtyQueue.push_back(position);
}

const std::vector<Chunk *> &World::getChunks() const
{
    return chunks;
}