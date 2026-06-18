#include <engine/world.hpp>

#include <iostream>

World::World() { Noise(); }

void World::updateChunkSphere(const glm::vec3 &origin, int radius, JobSystem &jobs)
{
    ChunkPosition currentOrigin = {static_cast<int>(std::floor(origin.x / 16.0f)), static_cast<int>(std::floor(origin.y / 16.0f)), static_cast<int>(std::floor(origin.z / 16.0f))};

    if (currentOrigin == previousOrigin && this->radius == radius) return;

    previousOrigin = currentOrigin;

    this->radius = radius;

    int squaredRadius = radius * radius;
    std::vector<ChunkPosition> loadList;
    std::vector<ChunkPosition> unloadList;

    {
        std::lock_guard<std::mutex> lock(mapMutex);

        for (auto index = chunkTable.begin(); index != chunkTable.end();)
        {
            ChunkPosition position = index->first;

            int dx = position.x - currentOrigin.x;
            int dy = position.y - currentOrigin.y;
            int dz = position.z - currentOrigin.z;

            if ((dx * dx + dy * dy + dz * dz) > squaredRadius)
            {
                unloadList.push_back(position);
            }

            ++index;
        }
    }

    for (const auto &position : unloadList)
    {
        unloadChunk(position);
    }

    {
        std::lock_guard<std::mutex> lock(mapMutex);

        for (int x = -radius; x <= radius; x++)
        {
            for (int y = -radius; y <= radius; y++)
            {
                for (int z = -radius; z <= radius; z++)
                {
                    if ((x * x + y * y + z * z) > squaredRadius) continue;

                    ChunkPosition target = {static_cast<int>(currentOrigin.x + x), static_cast<int>(currentOrigin.y + y), static_cast<int>(currentOrigin.z + z)};

                    if (!chunkTable.contains(target)) loadList.push_back(target);
                }
            }
        }
    }

    if (!loadList.empty()) loadChunks(loadList, jobs);
}

void updateAwaiting(std::shared_ptr<Chunk> chunk, World *world, JobSystem &jobs, const std::shared_ptr<Chunk> source)
{
    if (chunk == nullptr || chunk->state.load() >= ChunkState::Meshing) return;

    if (chunk->awaitingData.fetch_sub(1, std::memory_order_acq_rel) == 1)
    {
        chunk->state.store(ChunkState::Meshing, std::memory_order_release);

        jobs.push(
            [chunk, world]()
            {
                auto data = chunk->buildMesh();

                world->queueMesh(std::move(data));
            });
    }
}

void World::loadChunks(const std::vector<ChunkPosition> &positions, JobSystem &jobs)
{
    std::lock_guard<std::mutex> lock(mapMutex);

    std::vector<std::shared_ptr<Chunk>> generationQueue;

    for (const auto &position : positions)
    {
        if (chunkTable.contains(position)) continue;

        auto chunk = std::make_shared<Chunk>(position);

        chunkTable[position] = chunk;

        generationQueue.push_back(chunk);
    }

    for (auto &chunk : generationQueue)
    {
        ChunkPosition position = chunk->getPosition();

        for (int i = 0; i < 6; i++)
        {
            ChunkPosition neighborPosition = {
                position.x + Chunk::NEIGHBORS[i][0],
                position.y + Chunk::NEIGHBORS[i][1],
                position.z + Chunk::NEIGHBORS[i][2],
            };

            auto index = chunkTable.find(neighborPosition);
            if (index != chunkTable.end())
            {
                std::shared_ptr<Chunk> neighbor = index->second;

                chunk->neighbors[i] = index->second;
                index->second->neighbors[i ^ 1] = chunk;

                if (neighbor->state.load() >= ChunkState::AwaitingMesh)
                {
                    chunk->awaitingData.fetch_sub(1);
                }
            }
        }
    }

    for (auto &chunk : generationQueue)
    {
        chunk->state.store(ChunkState::Generation, std::memory_order_release);

        jobs.push(
            [chunk, this, &jobs]()
            {
                generateChunk(chunk);

                chunk->state.store(ChunkState::AwaitingMesh, std::memory_order_release);

                updateAwaiting(chunk, this, jobs, chunk);

                for (int i = 0; i < 6; i++)
                {
                    updateAwaiting(chunk->neighbors[i], this, jobs, chunk);
                }
            });
    }
}

void World::queueMesh(std::unique_ptr<ChunkUpload> data)
{
    std::lock_guard<std::mutex> lock(meshMutex);

    meshQueue.push_back(std::move(data));
}

void World::update(JobSystem &jobs)
{
    std::vector<std::unique_ptr<ChunkUpload>> meshUploads;

    {
        std::lock_guard<std::mutex> lock(meshMutex);

        if (!meshQueue.empty()) meshUploads.swap(meshQueue);
    }

    for (auto &meshData : meshUploads)
    {
        std::lock_guard<std::mutex> lock(mapMutex);

        auto index = chunkTable.find(meshData->position);
        if (index != chunkTable.end())
        {
            std::shared_ptr<Chunk> chunk = index->second;

            auto mesh = std::make_unique<Mesh>();

            mesh->upload(meshData->vertices, meshData->indices);

            chunk->setMesh(std::move(mesh));

            chunk->state.store(ChunkState::Ready, std::memory_order_release);

            chunk->index = chunks.size();
            chunks.push_back(chunk.get());
        }
    }
}

void World::draw(Shader &shader)
{
    shader.bind();

    for (auto chunk : chunks)
    {
        if (chunk != nullptr && chunk->state.load(std::memory_order_acquire) == ChunkState::Ready)
        {
            glm::vec3 worldPosition = chunk->getPosition().toVec3() * 16.0f;

            shader.setUniform("model", glm::translate(glm::mat4(1.0f), worldPosition));

            chunk->getMesh()->draw();
        }
    }
}

void World::unloadChunk(ChunkPosition position)
{
    std::lock_guard<std::mutex> lock(mapMutex);

    auto index = chunkTable.find(position);
    if (index != chunkTable.end())
    {
        Chunk *target = index->second.get();

        if (target->state.load(std::memory_order_acquire) == ChunkState::Ready && !chunks.empty())
        {
            // std::size_t swapIndex = target->index;

            // Chunk *lastChunk = chunks.back();

            // chunks[swapIndex] = lastChunk;
            // lastChunk->index = swapIndex;

            auto it = std::find(chunks.begin(), chunks.end(), target);
            if (it != chunks.end())
            {
                std::iter_swap(it, chunks.end() - 1);
                chunks.pop_back();
            }
        }

        for (int i = 0; i < 6; i++)
        {
            std::shared_ptr<Chunk> neighbor = index->second->neighbors[i];

            if (neighbor != nullptr)
            {
                if (target->state.load() >= ChunkState::AwaitingMesh)
                {
                    neighbor->awaitingData.fetch_add(1);
                }

                neighbor->neighbors[i ^ 1] = nullptr;
            }
        }

        chunkTable.erase(index);
    }
}

bool World::chunkExists(ChunkPosition position) const
{
    std::lock_guard<std::mutex> lock(mapMutex);

    return chunkTable.contains(position);
}

void World::generateChunk(const std::shared_ptr<Chunk> chunk)
{
    ChunkPosition position = chunk->getPosition();
    int wcx = position.x << 4;
    int wcy = position.y << 4;
    int wcz = position.z << 4;

    long seed = 46416616514;

    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int z = 0; z < 16; z++)
            {
                double wx = static_cast<double>(wcx + x);
                double wy = static_cast<double>(wcy + y);
                double wz = static_cast<double>(wcz + z);

                float hills = Noise::get2D(seed, wx * 0.01, wz * 0.01) * 30.0f;
                float bumps = Noise::get2D(seed, wx * 0.04, wz * 0.04) * 10.0f;

                char blockType = 0;

                if (wy < (hills + bumps + 10.0f))
                {
                    blockType = 1;
                }

                chunk->setBlock(x, y, z, blockType);
            }
        }
    }
}