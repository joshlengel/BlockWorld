#pragma once

#include"Core.h"
#include"Mesh.h"
#include"Vec.h"

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
        STONE,
        SAND,
        LOG,
        LEAVES
    };

    Type type;

    ui32 GetSideIndex() const;
    ui32 GetBottomIndex() const;
    ui32 GetTopIndex() const;
    bool IsTransparent() const;
};

class Chunk
{
public:
    Chunk(i32 x, i32 z);

    i32 GetX() const;
    i32 GetZ() const;

    bool Loaded() const;

    Voxel &GetBlock(const Vec3ui &pos);
    void SetBlock(const Vec3ui &pos, const Voxel &v);

    void GenerateMesh();
    void GenerateBorder(Face face, Chunk *neighbor);
    void Load();
    void Render();

private:
    i32 m_x, m_z;

    Voxel m_voxels[CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT];
    Mesh m_mesh;
    bool m_loaded;
};