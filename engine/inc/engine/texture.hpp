#pragma once

#include <glad/gl.h>
#include <stb_image.h>

#include <iostream>
#include <string>

class Texture
{
private:
    unsigned int handle;

public:
    Texture(const std::string &path);

    ~Texture();

    void bind(unsigned int slot = 0);
};