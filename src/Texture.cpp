#include"Texture.h"

#include<glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include<stb_image.h>

struct TextureData
{
    GLuint tex_id;
};

Texture::Texture():
    m_data(new TextureData)
{
    glGenTextures(1, &m_data->tex_id);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_data->tex_id);
    delete m_data;
}

void Texture::Bind(size_t slot) { glBindTexture(GL_TEXTURE_2D, m_data->tex_id); }

Texture Texture::FromPixels(size_t width, size_t height, const uint8_t *pixels)
{
    Texture texture;
    texture.Bind();

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return texture;
}

Texture Texture::FromImage(const std::string &file)
{
    int width, height;
    uint8_t *pixels = stbi_load(file.c_str(), &width, &height, nullptr, STBI_rgb_alpha);
    Texture texture = FromPixels(width, height, pixels);
    stbi_image_free(pixels);

    return texture;
}