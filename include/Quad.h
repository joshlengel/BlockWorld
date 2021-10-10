#pragma once

#include<vector>

class Quad
{
public:
    float x, y;
    float width, height;

    float r = 1, g = 1, b = 1, a = 1;
};

struct QuadRendererData;

class QuadRenderer
{
public:
    QuadRenderer();
    ~QuadRenderer();

    void Add(const Quad &quad);
    void Render();

private:
    QuadRendererData *m_data;

    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    size_t m_last_index;
};