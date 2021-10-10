#pragma once

#include<vector>
#include<cstdint>

#define TEXTURE_ATLAS_SIZE 4

struct MeshData;

enum class Face
{
    LEFT,
    RIGHT,
    BOTTOM,
    TOP,
    BACK,
    FRONT
};

class Mesh
{
public:
    Mesh();
    ~Mesh();

    void AddFace(Face face, uint16_t tex_index, size_t x, size_t y, size_t z);
    void Generate();
    void Render() const;

private:
    MeshData *m_data;

    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    size_t m_last_index, m_index_count;
};