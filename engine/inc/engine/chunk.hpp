#pragma once

#include <array>
#include <memory>

class Chunk
{
private:
    int x;
    int y;
    int z;

    static constexpr int SIZE = 16;
    static constexpr int VOLUME = SIZE * SIZE * SIZE;

    std::array<char, VOLUME> blocks;

public:
    Chunk(int x, int y, int z);

    ~Chunk();

    int getX();

    int getY();

    int getZ();

    void setBlock(int x, int y, int z, char type);

    char getBlock(int x, int y, int z);

private:
    inline int getIndex(int x, int y, int z) const;
};