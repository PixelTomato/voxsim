#include <engine/world.hpp>

void World::update(JobSystem &jobs)
{
    collectData();
    collectMeshes();

    scheduleGen(jobs);
    scheduleMeshes(jobs);

    unloadChunks();
}

void World::draw(Shader &shader)
{
    shader.bind();

    for (auto &[key, chunk] : renderChunks)
    {
        if (chunk.mesh.vertexCount > 0)
        {
            glm::vec3 position{key.x * 16.0f, key.y * 16.0f, key.z * 16.0f};
            shader.setUniform("model", glm::translate(glm::mat4(1.0f), position));

            chunk.mesh.draw();
        }
    }
}

void World::loadSphere(glm::vec3 origin, int radius)
{
    int chunkX = static_cast<int>(origin.x);
    int chunkY = static_cast<int>(origin.y);
    int chunkZ = static_cast<int>(origin.z);

    ChunkKey originKey{chunkX, chunkY, chunkZ};

    if (originKey == lastOrigin) return;
    lastOrigin = originKey;

    loadQueue.clear();
    unloadQueue.clear();

    std::unordered_set<ChunkKey, ChunkHash> sphereKeys;

    const int squareRad = radius * radius;

    for (int x = chunkX - radius; x <= chunkX + radius; x++)
    {
        for (int y = chunkY - radius; y <= chunkY + radius; y++)
        {
            for (int z = chunkZ - radius; z <= chunkZ + radius; z++)
            {
                int dx = x - chunkX;
                int dy = y - chunkY;
                int dz = z - chunkZ;

                if ((dx * dx + dy * dy + dz * dz) < squareRad)
                {
                    ChunkKey key{x, y, z};

                    sphereKeys.insert(key);

                    if (!loadedKeys.contains(key))
                    {
                        loadQueue.push_back(key);
                    }
                }
            }
        }
    }

    for (const auto &key : loadedKeys)
    {
        if (!sphereKeys.contains(key)) unloadQueue.push_back(key);
    }

    loadedKeys = std::move(sphereKeys);
}

void World::collectData()
{
    auto data = genJobOutput.take();

    for (auto dataChunk : data)
    {
        genStage.erase(dataChunk->key);

        dataChunks.emplace(dataChunk->key, std::move(dataChunk));
    }
}

void World::collectMeshes()
{
    auto data = meshJobOutput.take();

    for (auto &meshChunk : data)
    {
        pendingUploads.push_back(std::move(meshChunk));
    }

    for (int i = 0; i < 16 && !pendingUploads.empty(); i++)
    {
        auto meshChunk = pendingUploads.back();
        pendingUploads.pop_back();

        meshStage.erase(meshChunk.key);

        RenderChunk renderChunk;
        renderChunk.key = meshChunk.key;
        renderChunk.mesh.upload(meshChunk.vertices, meshChunk.indices);

        renderChunks[meshChunk.key] = std::move(renderChunk);
    }
}

void World::scheduleGen(JobSystem &jobs)
{
    for (const auto &key : loadedKeys)
    {
        if (dataChunks.contains(key) || genStage.contains(key)) continue;

        genStage.insert(key);

        jobs.push(
            [this, key]
            {
                auto dataChunk = std::make_shared<DataChunk>();
                dataChunk->key = key;

                generateChunk(dataChunk);

                genJobOutput.push(std::move(dataChunk));
            });
    }
}

bool World::canMesh(ChunkKey key) const
{
    constexpr int offsets[6][3] = {{+0, +0, +1}, {+0, +0, -1}, {+1, +0, +0}, {-1, +0, +0}, {+0, +1, +0}, {+0, -1, +0}};

    for (auto offset : offsets)
    {
        if (!dataChunks.contains({key.x + offset[0], key.y + offset[1], key.z + offset[2]})) return false;
    }

    return true;
}

void World::scheduleMeshes(JobSystem &jobs)
{
    for (auto &[key, chunk] : dataChunks)
    {
        if (renderChunks.contains(key) || meshStage.contains(key)) continue;

        if (canMesh(key))
        {
            meshStage.insert(key);

            ChunkPack chunkPack;
            chunkPack.main = chunk;

            for (int i = 0; i < 6; i++)
            {
                ChunkKey neighborKey;
                neighborKey.x = key.x + Chunk::NEIGHBORS[i][0];
                neighborKey.y = key.y + Chunk::NEIGHBORS[i][1];
                neighborKey.z = key.z + Chunk::NEIGHBORS[i][2];

                chunkPack.neighbors[i] = dataChunks.at(neighborKey);
            }

            jobs.push(
                [this, key, chunkPack]
                {
                    MeshChunk meshChunk;
                    meshChunk.key = key;

                    Chunk::buildMesh(key, chunkPack, meshChunk);

                    meshJobOutput.push(std::move(meshChunk));
                });
        }
    }
}

void World::unloadChunks()
{
    for (auto iterator = renderChunks.begin(); iterator != renderChunks.end();)
    {
        if (!loadedKeys.contains(iterator->first))
        {
            iterator = renderChunks.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }

    for (auto iterator = dataChunks.begin(); iterator != dataChunks.end();)
    {
        if (!loadedKeys.contains(iterator->first))
        {
            iterator = dataChunks.erase(iterator);
        }
        else
        {
            ++iterator;
        }
    }
}

void World::generateChunk(std::shared_ptr<DataChunk> chunk)
{
    int wChunkX = chunk->key.x * 16;
    int wChunkY = chunk->key.y * 16;
    int wChunkZ = chunk->key.z * 16;

    long seed = 46416616514;

    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int z = 0; z < 16; z++)
            {
                double wx = static_cast<double>(wChunkX + x);
                double wy = static_cast<double>(wChunkY + y);
                double wz = static_cast<double>(wChunkZ + z);

                float hills = Noise::get2D(seed, wx * 0.0075, wz * 0.0075) * 15.0f;
                float bumps = Noise::get2D(seed, (wx + 20.0f) * 0.05, (wz + 20.0f) * 0.05) * 4.0f;

                char blockType = 0;

                if (wy < (hills + bumps))
                {
                    blockType = static_cast<int>(glm::mod(static_cast<float>(wy), 32.0f)) + 1;

                    chunk->voxels++;
                }

                chunk->blocks[Chunk::getIndex(x, y, z)] = blockType;
            }
        }
    }
}