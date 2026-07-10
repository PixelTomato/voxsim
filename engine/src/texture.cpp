#include <engine/texture.hpp>

Texture::Texture(const std::string &path)
{
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(false);

    int width;
    int height;
    int channelCount;

    unsigned char *data = stbi_load(path.c_str(), &width, &height, &channelCount, 0);

    if (data)
    {
        GLenum format = (channelCount == 4) ? GL_RGBA : GL_RGB;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture: " << path << "\n";
    }

    stbi_image_free(data);
}

Texture::~Texture() { glDeleteTextures(1, &handle); }

void Texture::bind(unsigned int slot)
{
    glActiveTexture(GL_TEXTURE0 + slot);

    glBindTexture(GL_TEXTURE_2D, handle);
}