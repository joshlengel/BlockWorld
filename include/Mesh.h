#pragma once

#include"Core.h"

#include<vector>

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

    void AddFace(Face face, ui32 tex_index, i32 x, i32 y, i32 z);
    void Load();
    void Flush();
    void Render() const;

private:
    MeshData *m_data;

    std::vector<float> m_vertices;
    std::vector<unsigned int> m_indices;
    size_t m_last_index, m_index_count;
};