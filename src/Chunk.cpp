#include"Chunk.h"

#include<cstddef>

#define INDEX(x,y,z) ((y) * CHUNK_WIDTH * CHUNK_WIDTH + (z) * CHUNK_WIDTH + (x))

static const uint16_t VOXEL_INDICES[] =
{
    0, 0, 0,
    1, 1, 1,
    0, 1, 2,
    3, 3, 3
};

uint16_t Voxel::GetSideIndex() const { return VOXEL_INDICES[3 * static_cast<uint16_t>(type)]; }
uint16_t Voxel::GetBottomIndex() const { return VOXEL_INDICES[3 * static_cast<uint16_t>(type) + 1]; }
uint16_t Voxel::GetTopIndex() const { return VOXEL_INDICES[3 * static_cast<uint16_t>(type) + 2]; }

Chunk::Chunk(int32_t x, int32_t z):
    m_x(x), m_z(z)
{
    GenerateMesh();
}

void Chunk::GenerateMesh()
{
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    for (size_t z = 0; z < CHUNK_WIDTH; ++z)
    for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
    {
        size_t height = x + 10;

        Voxel::Type type;
        if (y < height - 4) type = Voxel::Type::STONE;
        else if (y < height - 1) type = Voxel::Type::DIRT;
        else if (y < height) type = Voxel::Type::GRASS;
        else type = Voxel::Type::AIR;

        m_voxels[INDEX(x, y, z)].type = type;
    }

    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    for (size_t y = 0; y < CHUNK_HEIGHT; ++y)
    for (size_t z = 0; z < CHUNK_WIDTH; ++z)
    {
        Voxel &v = m_voxels[INDEX(x, y, z)];
        if (v.type == Voxel::Type::AIR) continue;
        if (x == 0                || m_voxels[INDEX(x-1, y, z)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::LEFT, v.GetSideIndex(), x, y, z);
        if (x == CHUNK_WIDTH - 1  || m_voxels[INDEX(x+1, y, z)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::RIGHT, v.GetSideIndex(), x, y, z);
        if (y == 0                || m_voxels[INDEX(x, y-1, z)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::BOTTOM, v.GetBottomIndex(), x, y, z);
        if (y == CHUNK_HEIGHT - 1 || m_voxels[INDEX(x, y+1, z)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::TOP, v.GetTopIndex(), x, y, z);
        if (z == 0                || m_voxels[INDEX(x, y, z-1)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::BACK, v.GetSideIndex(), x, y, z);
        if (z == CHUNK_WIDTH - 1  || m_voxels[INDEX(x, y, z+1)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::FRONT, v.GetSideIndex(), x, y, z);
    }

    m_mesh.Generate();
}

void Chunk::Render()
{
    m_mesh.Render();
}