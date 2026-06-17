#include <engine/chunk.hpp>

#include <engine/world.hpp>

std::unique_ptr<ChunkUpload> Chunk::buildMesh()
{
    auto data = std::make_unique<ChunkUpload>(position);

    const float cubeFaces[6][4][8] = {
        {{0, 0, 1, +0, +0, +1, 0.0f, 0.0f}, {1, 0, 1, +0, +0, +1, 1.0f, 0.0f}, {1, 1, 1, +0, +0, +1, 1.0f, 1.0f}, {0, 1, 1, +0, +0, +1, 0.0f, 1.0f}},                                                                                                                                               // front
        {{1, 0, 0, +0, +0, -1, 0.0f, 0.0f}, {0, 0, 0, +0, +0, -1, 1.0f, 0.0f}, {0, 1, 0, +0, +0, -1, 1.0f, 1.0f}, {1, 1, 0, +0, +0, -1, 0.0f, 1.0f}}, {{1, 0, 1, +1, +0, +0, 0.0f, 0.0f}, {1, 0, 0, +1, +0, +0, 1.0f, 0.0f}, {1, 1, 0, +1, +0, +0, 1.0f, 1.0f}, {1, 1, 1, +1, +0, +0, 0.0f, 1.0f}}, // right
        {{0, 0, 0, -1, +0, +0, 0.0f, 0.0f}, {0, 0, 1, -1, +0, +0, 1.0f, 0.0f}, {0, 1, 1, -1, +0, +0, 1.0f, 1.0f}, {0, 1, 0, -1, +0, +0, 0.0f, 1.0f}}, {{0, 1, 1, +0, +1, +0, 0.0f, 0.0f}, {1, 1, 1, +0, +1, +0, 1.0f, 0.0f}, {1, 1, 0, +0, +1, +0, 1.0f, 1.0f}, {0, 1, 0, +0, +1, +0, 0.0f, 1.0f}}, // top
        {{0, 0, 0, +0, -1, +0, 0.0f, 0.0f}, {1, 0, 0, +0, -1, +0, 1.0f, 0.0f}, {1, 0, 1, +0, -1, +0, 1.0f, 1.0f}, {0, 0, 1, +0, -1, +0, 0.0f, 1.0f}},
    };

    int indexOffset = 0;

    for (int x = 0; x < 16; x++)
    {
        for (int y = 0; y < 16; y++)
        {
            for (int z = 0; z < 16; z++)
            {
                char type = getBlock(x, y, z);

                if (type == 0) continue;

                for (int face = 0; face < 6; face++)
                {
                    int nx = x + NEIGHBORS[face][0];
                    int ny = y + NEIGHBORS[face][1];
                    int nz = z + NEIGHBORS[face][2];

                    bool inBounds = (nx >= 0 && nx < 16) && (ny >= 0 && ny < 16) && (nz >= 0 && nz < 16);

                    bool visible = false;

                    if (inBounds)
                    {
                        if (getBlock(nx, ny, nz) == 0) visible = true;
                    }
                    else
                    {
                        std::shared_ptr<Chunk> neighbor = neighbors[face];

                        if (neighbor == nullptr)
                        {
                            visible = true;
                        }
                        else if (neighbor->getBlock(nx & 15, ny & 15, nz & 15) == 0)
                        {
                            visible = true;
                        }
                    }

                    if (visible)
                    {
                        for (int vertex = 0; vertex < 4; vertex++)
                        {
                            data->vertices.push_back(x + cubeFaces[face][vertex][0]);
                            data->vertices.push_back(y + cubeFaces[face][vertex][1]);
                            data->vertices.push_back(z + cubeFaces[face][vertex][2]);

                            data->vertices.push_back(cubeFaces[face][vertex][3]);
                            data->vertices.push_back(cubeFaces[face][vertex][4]);
                            data->vertices.push_back(cubeFaces[face][vertex][5]);

                            data->vertices.push_back(cubeFaces[face][vertex][6]);
                            data->vertices.push_back(cubeFaces[face][vertex][7]);
                        }

                        data->indices.push_back(indexOffset + 0);
                        data->indices.push_back(indexOffset + 1);
                        data->indices.push_back(indexOffset + 2);
                        data->indices.push_back(indexOffset + 2);
                        data->indices.push_back(indexOffset + 3);
                        data->indices.push_back(indexOffset + 0);

                        indexOffset += 4;
                    }
                }
            }
        }
    }

    return data;
}