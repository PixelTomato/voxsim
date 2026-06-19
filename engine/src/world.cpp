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
        glm::vec3 position{key.x * 16.0f, key.y * 16.0f, key.z * 16.0f};
        shader.setUniform("model", glm::translate(glm::mat4(1.0f), position));

        chunk.mesh.draw();
    }
}

void World::loadSphere(glm::vec3 origin, int radius)
{
    loadedKeys.clear();

    int cx = static_cast<int>(origin.x / 16.0f);
    int cy = static_cast<int>(origin.y / 16.0f);
    int cz = static_cast<int>(origin.z / 16.0f);

    for (int x = cx - radius; x <= cx + radius; x++)
    {
        for (int y = cy - radius; y <= cy + radius; y++)
        {
            for (int z = cz - radius; z <= cz + radius; z++)
            {
                int dx = x - cx;
                int dy = y - cy;
                int dz = z - cz;

                if ((dx * dx + dy * dy + dz * dz) < (radius * radius))
                {
                    loadedKeys.insert({x, y, z});
                }
            }
        }
    }
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
    const int offsets[6][3] = {{+0, +0, +1}, {+0, +0, -1}, {+1, +0, +0}, {-1, +0, +0}, {+0, +1, +0}, {+0, -1, +0}};

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
    int wcx = chunk->key.x * 16;
    int wcy = chunk->key.y * 16;
    int wcz = chunk->key.z * 16;

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

                float hills = Noise::get2D(seed, wx * 0.01, wz * 0.01) * 20.0f;
                float bumps = Noise::get2D(seed, wx * 0.04, wz * 0.04) * 10.0f;

                char blockType = 0;

                if (wy < (hills + bumps))
                {
                    blockType = 1;
                }

                chunk->blocks[Chunk::getIndex(x, y, z)] = blockType;
            }
        }
    }
}