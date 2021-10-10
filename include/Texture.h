#pragma once

#include<string>

struct TextureData;

class Texture
{
public:
    Texture();
    ~Texture();

    void Bind(size_t slot = 0);

    static Texture FromPixels(size_t width, size_t height, const uint8_t *pixels);
    static Texture FromImage(const std::string &file);

private:
    TextureData *m_data;
};