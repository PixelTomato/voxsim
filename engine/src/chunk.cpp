#include <engine/chunk.hpp>

Chunk::Chunk(int x, int y, int z)
{
    this->x = x;
    this->y = y;
    this->z = z;
}

Chunk::~Chunk()
{
}

int Chunk::getX()
{
    return x;
}

int Chunk::getY()
{
    return y;
}

int Chunk::getZ()
{
    return z;
}

void Chunk::setBlock(int x, int y, int z, char type)
{
    blocks[getIndex(x, y, z)] = type;
}

char Chunk::getBlock(int x, int y, int z)
{
    return blocks[getIndex(x, y, z)];
}

inline int Chunk::getIndex(int x, int y, int z) const
{
    return x + (z + y * SIZE) * SIZE;
}