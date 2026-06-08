#include <engine/world.hpp>

#include <iostream>

World::World()
{
    workerThread = std::thread(&World::workerLoop, this);
}

World::~World()
{
    isRunning = false;

    if (workerThread.joinable())
    {
        workerThread.join();
    }
}

void World::workerLoop()
{
    while (isRunning)
    {
        ChunkPosition target;

        bool hasWork = false;

        {
            std::lock_guard<std::mutex> lock(queueMutex);

            if (!genQueue.empty())
            {
                target = genQueue.back();

                genQueue.pop_back();

                hasWork = true;
            }
        }

        if (hasWork)
        {
            Chunk *chunk = getChunkThreadSafe(target);

            if (chunk != nullptr)
            {
                generateChunk(chunk);

                {
                    std::lock_guard<std::mutex> lock(mapMutex);
                    chunk->setGenerated(true);
                }

                {
                    std::lock_guard<std::mutex> lock(queueMutex);

                    for (int i = 0; i < 6; i++)
                    {
                        if (chunk->neighbors[i] != nullptr)
                        {
                            if (chunk->neighbors[i]->isGenerated())
                            {
                                chunk->neighbors[i]->markDirty(*this);
                            }
                        }
                    }

                    chunk->markDirty(*this);
                }
            }
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(1));
        }
    }
}

Chunk *World::getChunkThreadSafe(ChunkPosition position)
{
    std::lock_guard<std::mutex> lock(mapMutex);

    auto target = chunkTable.find(position);
    if (target != chunkTable.end())
    {
        return target->second.get();
    }

    return nullptr;
}

void World::update()
{
    int remeshed = 0;

    while (remeshed < 12)
    {
        ChunkPosition position;

        bool hasWork = false;

        {
            std::lock_guard<std::mutex> lock(queueMutex);
            if (!dirtyQueue.empty())
            {
                position = dirtyQueue.back();

                dirtyQueue.pop_back();

                hasWork = true;
            }
        }

        if (!hasWork) break;

        {
            std::lock_guard<std::mutex> lock(mapMutex);

            if (isLoaded(position))
            {
                Chunk *chunk = chunkTable[position].get();

                chunk->rebuildMesh();

                remeshed++;
            }
        }
    }
}

void World::draw(Shader &shader)
{
    std::lock_guard<std::mutex> lock(mapMutex);

    for (const auto chunk : chunks)
    {
        if (chunk->isMeshed())
        {
            shader.setUniform("model", glm::translate(glm::mat4(1.0f), chunk->getPosition().toVec3() * 16.0f));

            chunk->getMesh()->draw();
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

void World::updateRadius(glm::vec3 cameraPosition, int radius)
{
    int cameraChunkX = std::floor(cameraPosition.x / 16.0f);
    int cameraChunkY = std::floor(cameraPosition.y / 16.0f);
    int cameraChunkZ = std::floor(cameraPosition.z / 16.0f);

    for (int x = cameraChunkX - radius; x <= cameraChunkX + radius; x++)
    {
        for (int y = cameraChunkY - radius; y <= cameraChunkY + radius; y++)
        {
            for (int z = cameraChunkZ - radius; z <= cameraChunkZ + radius; z++)
            {
                int dx = cameraChunkX - x;
                int dy = cameraChunkY - y;
                int dz = cameraChunkZ - z;

                if ((dx * dx + dy * dy + dz * dz) <= (radius * radius))
                {
                    loadChunk({x, y, z});
                }
            }
        }
    }

    for (int i = chunks.size() - 1; i >= 0; i--)
    {
        ChunkPosition chunk = chunks[i]->getPosition();

        int dx = cameraChunkX - chunk.x;
        int dy = cameraChunkY - chunk.y;
        int dz = cameraChunkZ - chunk.z;

        int unloadRadius = radius + 1;

        if ((dx * dx + dy * dy + dz * dz) > (unloadRadius * unloadRadius))
        {
            unloadChunk(chunk);
        }
    }
}

void World::loadChunk(ChunkPosition position)
{
    {
        std::lock_guard<std::mutex> lock(mapMutex);

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
    }

    {
        std::lock_guard<std::mutex> lock(queueMutex);
        genQueue.push_back(position);
    }
}

void World::unloadChunk(ChunkPosition position)
{
    std::lock_guard<std::mutex> lock(mapMutex);

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
    std::lock_guard<std::mutex> lock(mapMutex);

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