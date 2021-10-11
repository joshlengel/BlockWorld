#pragma once

#include"Texture.h"
#include"Shader.h"

#include<vector>
#include<map>

class Quad
{
public:
    float x, y;
    float width, height;

    float r = 1, g = 1, b = 1, a = 1;

    Texture *texture = nullptr;
};

struct QuadRendererData;

class QuadRenderer
{
public:
    QuadRenderer();
    ~QuadRenderer();

    void Add(const Quad &quad);
    void Flush();
    void Render();

private:
    QuadRendererData *m_data;
    Shader m_shader;

    std::vector<Quad> m_quads;

    Texture m_default_texture;
};