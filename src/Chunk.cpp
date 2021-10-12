#include"Chunk.h"

#include<cstddef>
#include<cstring>
#include<stdexcept>

#define INDEX(x,y,z) ((y) * CHUNK_WIDTH * CHUNK_WIDTH + (z) * CHUNK_WIDTH + (x))

static const ui32 VOXEL_INDICES[] =
{
    0, 0, 0,
    1, 1, 1,
    0, 1, 2,
    3, 3, 3,
    4, 4, 4,
    5, 6, 6,
    7, 7, 7
};

static const bool VOXEL_TRANSPARENT[] =
{
    true,
    false,
    false,
    false,
    false,
    false,
    true
};

ui32 Voxel::GetSideIndex() const { return VOXEL_INDICES[3 * static_cast<ui32>(type)]; }
ui32 Voxel::GetBottomIndex() const { return VOXEL_INDICES[3 * static_cast<ui32>(type) + 1]; }
ui32 Voxel::GetTopIndex() const { return VOXEL_INDICES[3 * static_cast<ui32>(type) + 2]; }
bool Voxel::IsTransparent() const { return VOXEL_TRANSPARENT[static_cast<ui32>(type)]; }

Chunk::Chunk(i32 x, i32 z):
    m_x(x), m_z(z),
    m_loaded(false)
{
    std::memset(m_voxels, 0, sizeof(m_voxels));
}

i32 Chunk::GetX() const { return m_x; }
i32 Chunk::GetZ() const { return m_z; }
bool Chunk::Loaded() const { return m_loaded; }

Voxel &Chunk::GetBlock(const Vec3ui &pos) { return m_voxels[INDEX(pos.x,pos.y,pos.z)]; }
void Chunk::SetBlock(const Vec3ui &pos, const Voxel &v) { m_voxels[INDEX(pos.x,pos.y,pos.z)] = v; }

void Chunk::GenerateMesh()
{
    m_mesh.Flush();
    for (ui32 x = 0; x < CHUNK_WIDTH; ++x)
    for (ui32 y = 0; y < CHUNK_HEIGHT; ++y)
    for (ui32 z = 0; z < CHUNK_WIDTH; ++z)
    {
        Voxel &v = m_voxels[INDEX(x, y, z)];
        if (v.type == Voxel::Type::AIR) continue;

        i32 ax = m_x + static_cast<i32>(x);
        i32 ay = static_cast<i32>(y);
        i32 az = m_z + static_cast<i32>(z);

        if (x > 0 && m_voxels[INDEX(x-1, y, z)].IsTransparent())                m_mesh.AddFace(Face::LEFT, v.GetSideIndex(), ax, ay, az);
        if (x < CHUNK_WIDTH - 1 && m_voxels[INDEX(x+1, y, z)].IsTransparent())  m_mesh.AddFace(Face::RIGHT, v.GetSideIndex(), ax, ay, az);
        if (y > 0 && m_voxels[INDEX(x, y-1, z)].IsTransparent())                m_mesh.AddFace(Face::BOTTOM, v.GetBottomIndex(), ax, ay, az);
        if (y < CHUNK_HEIGHT - 1 && m_voxels[INDEX(x, y+1, z)].IsTransparent()) m_mesh.AddFace(Face::TOP, v.GetTopIndex(), ax, ay, az);
        if (z > 0 && m_voxels[INDEX(x, y, z-1)].IsTransparent())                m_mesh.AddFace(Face::BACK, v.GetSideIndex(), ax, ay, az);
        if (z < CHUNK_WIDTH - 1 && m_voxels[INDEX(x, y, z+1)].IsTransparent())  m_mesh.AddFace(Face::FRONT, v.GetSideIndex(), ax, ay, az);
    }
    m_loaded = false;
}

void Chunk::GenerateBorder(Face face, Chunk *neighbor)
{
    switch (face)
    {
        case Face::LEFT:
        {
            for (ui32 y = 0; y < CHUNK_HEIGHT; ++y)
            for (ui32 z = 0; z < CHUNK_WIDTH; ++z)
            {
                Voxel &v = m_voxels[INDEX(0, y, z)];
                if (v.type == Voxel::Type::AIR) continue;

                i32 ax = m_x;
                i32 ay = static_cast<i32>(y);
                i32 az = m_z + static_cast<i32>(z);
                if (neighbor->m_voxels[INDEX(CHUNK_WIDTH - 1, y, z)].IsTransparent()) m_mesh.AddFace(Face::LEFT, v.GetSideIndex(), ax, ay, az);
            }
            break;
        }

        case Face::RIGHT:
        {
            for (ui32 y = 0; y < CHUNK_HEIGHT; ++y)
            for (ui32 z = 0; z < CHUNK_WIDTH; ++z)
            {
                Voxel &v = m_voxels[INDEX(CHUNK_WIDTH - 1, y, z)];
                if (v.type == Voxel::Type::AIR) continue;

                i32 ax = m_x + CHUNK_WIDTH - 1;
                i32 ay = static_cast<i32>(y);
                i32 az = m_z + static_cast<i32>(z);
                if (neighbor->m_voxels[INDEX(0, y, z)].IsTransparent()) m_mesh.AddFace(Face::RIGHT, v.GetSideIndex(), ax, ay, az);
            }
            break;
        }

        case Face::BACK:
        {
            for (ui32 x = 0; x < CHUNK_WIDTH; ++x)
            for (ui32 y = 0; y < CHUNK_HEIGHT; ++y)
            {
                Voxel &v = m_voxels[INDEX(x, y, 0)];
                if (v.type == Voxel::Type::AIR) continue;

                i32 ax = m_x + static_cast<i32>(x);
                i32 ay = static_cast<i32>(y);
                i32 az = m_z;
                if (neighbor->m_voxels[INDEX(x, y, CHUNK_WIDTH - 1)].IsTransparent()) m_mesh.AddFace(Face::BACK, v.GetSideIndex(), ax, ay, az);
            }
            break;
        }

        case Face::FRONT:
        {
            for (ui32 x = 0; x < CHUNK_WIDTH; ++x)
            for (ui32 y = 0; y < CHUNK_HEIGHT; ++y)
            {
                Voxel &v = m_voxels[INDEX(x, y, CHUNK_WIDTH - 1)];
                if (v.type == Voxel::Type::AIR) continue;

                i32 ax = m_x + static_cast<i32>(x);
                i32 ay = static_cast<i32>(y);
                i32 az = m_z + CHUNK_WIDTH - 1;
                if (neighbor->m_voxels[INDEX(x, y, 0)].IsTransparent()) m_mesh.AddFace(Face::FRONT, v.GetSideIndex(), ax, ay, az);
            }
            break;
        }
        
        default: throw std::runtime_error("Faces BACK and FRONT unsupported while generating neighboring chunk borders");
    }
    m_loaded = false;
}

void Chunk::Load()
{
    m_mesh.Load();
    m_loaded = true;
}

void Chunk::Render()
{
    m_mesh.Render();
}