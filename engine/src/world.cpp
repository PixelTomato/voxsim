#include <engine/world.hpp>

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

    generateChunk(chunk.get(), position);

    chunkTable[position] = chunks.size();

    chunks.push_back(std::move(chunk));
}

void World::unloadChunk(ChunkPosition position)
{
}

char World::getBlock(int x, int y, int z) const
{
}

void World::setBlock(int x, int y, int z, char type)
{
}

void World::generateChunk(Chunk *chunk, ChunkPosition position)
{
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
}

const std::vector<std::unique_ptr<Chunk>> &World::getChunks() const
{
    return chunks;
}