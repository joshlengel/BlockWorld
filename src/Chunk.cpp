#include"Chunk.h"

#include<glad/glad.h>

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

static bool ShouldAddFace(const Voxel &v, const Voxel &other)
{
    if (v.type == Voxel::Type::AIR) return false;
    if (!v.IsTransparent() && !other.IsTransparent()) return false;

    if (v.type == Voxel::Type::LEAVES && other.type == Voxel::Type::LEAVES) return false;
    if (v.type == Voxel::Type::LEAVES && !other.IsTransparent()) return false;

    return true;
}

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

void Chunk::AddFace(Face face, i32 x, i32 y, i32 z, const Voxel &v)
{
    Mesh &mesh = v.IsTransparent()? m_transparent_mesh : m_solid_mesh;
    ui32 tex_index;
    switch (face)
    {
        case Face::LEFT:
        case Face::RIGHT:
        case Face::BACK:
        case Face::FRONT:
            tex_index = v.GetSideIndex();
            break;
        
        case Face::BOTTOM:
            tex_index = v.GetBottomIndex();
            break;

        case Face::TOP:
            tex_index = v.GetTopIndex();
            break;
    }

    mesh.AddFace(face, tex_index, x, y, z);
}

void Chunk::GenerateMesh()
{
    m_solid_mesh.Flush();
    m_transparent_mesh.Flush();

    for (ui32 x = 0; x < CHUNK_WIDTH; ++x)
    for (ui32 y = 0; y < CHUNK_HEIGHT; ++y)
    for (ui32 z = 0; z < CHUNK_WIDTH; ++z)
    {
        Voxel &v = m_voxels[INDEX(x, y, z)];
        if (v.type == Voxel::Type::AIR) continue;

        i32 ax = m_x + static_cast<i32>(x);
        i32 ay = static_cast<i32>(y);
        i32 az = m_z + static_cast<i32>(z);

        if (x > 0 && ShouldAddFace(v, m_voxels[INDEX(x-1, y, z)]))                AddFace(Face::LEFT, ax, ay, az, v);
        if (x < CHUNK_WIDTH - 1 && ShouldAddFace(v, m_voxels[INDEX(x+1, y, z)]))  AddFace(Face::RIGHT, ax, ay, az, v);
        if (y > 0 && ShouldAddFace(v, m_voxels[INDEX(x, y-1, z)]))                AddFace(Face::BOTTOM, ax, ay, az, v);
        if (y < CHUNK_HEIGHT - 1 && ShouldAddFace(v, m_voxels[INDEX(x, y+1, z)])) AddFace(Face::TOP, ax, ay, az, v);
        if (z > 0 && ShouldAddFace(v, m_voxels[INDEX(x, y, z-1)]))                AddFace(Face::BACK, ax, ay, az, v);
        if (z < CHUNK_WIDTH - 1 && ShouldAddFace(v, m_voxels[INDEX(x, y, z+1)]))  AddFace(Face::FRONT, ax, ay, az, v);
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
                if (ShouldAddFace(v, neighbor->m_voxels[INDEX(CHUNK_WIDTH - 1, y, z)])) AddFace(Face::LEFT, ax, ay, az, v);
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
                if (ShouldAddFace(v, neighbor->m_voxels[INDEX(0, y, z)])) AddFace(Face::RIGHT, ax, ay, az, v);
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
                if (ShouldAddFace(v, neighbor->m_voxels[INDEX(x, y, CHUNK_WIDTH - 1)])) AddFace(Face::BACK, ax, ay, az, v);
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
                if (ShouldAddFace(v, neighbor->m_voxels[INDEX(x, y, 0)])) AddFace(Face::FRONT, ax, ay, az, v);
            }
            break;
        }
        
        default: throw std::runtime_error("Faces BACK and FRONT unsupported while generating neighboring chunk borders");
    }
    m_loaded = false;
}

void Chunk::Load()
{
    m_solid_mesh.Load();
    m_transparent_mesh.Load();
    m_loaded = true;
}

void Chunk::Render()
{
    glEnable(GL_CULL_FACE);
    m_solid_mesh.Render();
    glDisable(GL_CULL_FACE);
    m_transparent_mesh.Render();
}