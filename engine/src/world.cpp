#include <engine/world.hpp>

#include <iostream>

World::World() { Noise(); }

void updateAwaiting(std::shared_ptr<Chunk> chunk, World *world, JobSystem &jobs)
{
    if (chunk == nullptr) return;

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
            ChunkPosition neighbor = {
                position.x + Chunk::NEIGHBORS[i][0],
                position.y + Chunk::NEIGHBORS[i][1],
                position.z + Chunk::NEIGHBORS[i][2],
            };

            auto index = chunkTable.find(neighbor);
            if (index != chunkTable.end())
            {
                chunk->neighbors[i] = index->second;

                index->second->neighbors[i ^ 1] = chunk;
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

                updateAwaiting(chunk, this, jobs);

                for (int i = 0; i < 6; i++)
                {
                    updateAwaiting(chunk->neighbors[i], this, jobs);
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

            chunks.push_back(chunk.get());
        }
    }
}

void World::draw(Shader &shader)
{
    shader.bind();

    for (Chunk *chunk : chunks)
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

    std::erase_if(chunks, [position](Chunk *chunk) { return chunk->getPosition() == position; });

    auto index = chunkTable.find(position);
    if (index != chunkTable.end())
    {
        for (int i = 0; i < 6; i++)
        {
            if (index->second->neighbors[i] != nullptr)
            {
                index->second->neighbors[i]->neighbors[i ^ 1] = nullptr;
            }
        }

        chunkTable.erase(index);
    }
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