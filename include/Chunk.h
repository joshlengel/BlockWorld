#pragma once

#include"Mesh.h"

#include<cstdint>

#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 256

class Voxel
{
public:
    enum class Type
    {
        AIR,
        DIRT,
        GRASS,
        STONE
    };

    Type type;

    uint16_t GetSideIndex() const;
    uint16_t GetBottomIndex() const;
    uint16_t GetTopIndex() const;
};

class Chunk
{
public:
    Chunk(int32_t x, int32_t z);

    int32_t GetX() const;
    int32_t GetZ() const;

    bool Loaded() const;

    void SetBlock(size_t x, size_t y, size_t z, const Voxel &v);

    void GenerateMesh();
    void GenerateBorder(Face face, Chunk *neighbor);
    void Load();
    void Render();

private:
    int32_t m_x, m_z;

    Voxel m_voxels[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT];
    Mesh m_mesh;
    bool m_loaded;
};