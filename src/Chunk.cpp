#include"Chunk.h"
#include"Terrain.h"

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
    7, 7, 7,
    8, 8, 8
};

static const bool VOXEL_TRANSPARENT[] =
{
    true,
    false,
    false,
    false,
    false,
    false,
    true,
    true
};

static bool ShouldAddFace(Face face, const Voxel &v, const Voxel &other)
{
    if (v.type == Voxel::Type::AIR) return false;
    if (!v.IsTransparent() && !other.IsTransparent()) return false;

    if (v.type == Voxel::Type::LEAVES && other.type == Voxel::Type::LEAVES) return false;
    if (v.type == Voxel::Type::LEAVES && !other.IsTransparent()) return false;
    if (v.type == Voxel::Type::WATER && other.type != Voxel::Type::AIR && (other.type == Voxel::Type::WATER || face != Face::TOP)) return false;

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

void Chunk::AddFace(Face face, i32 x, i32 y, i32 z, const Voxel &v, float y_height)
{
    Mesh &mesh = v.type == Voxel::Type::WATER? m_water_mesh : (v.IsTransparent()? m_transparent_mesh : m_solid_mesh);
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

    mesh.AddFace(face, tex_index, x, y, z, y_height);
}

void Chunk::GenerateMesh()
{
    m_solid_mesh.Flush();
    m_transparent_mesh.Flush();
    m_water_mesh.Flush();

    for (ui32 x = 0; x < CHUNK_WIDTH; ++x)
    for (ui32 y = 0; y < CHUNK_HEIGHT; ++y)
    for (ui32 z = 0; z < CHUNK_WIDTH; ++z)
    {
        Voxel &v = m_voxels[INDEX(x, y, z)];
        if (v.type == Voxel::Type::AIR) continue;

        float y_height = 1.0f;
        bool should_add_top_face = y < CHUNK_HEIGHT - 1 && ShouldAddFace(Face::TOP, v, m_voxels[INDEX(x, y+1, z)]);
        if (v.type == Voxel::Type::WATER && should_add_top_face) y_height = 1.0f - WATER_HEIGHT_OFFSET; 

        i32 ax = m_x + static_cast<i32>(x);
        i32 ay = static_cast<i32>(y);
        i32 az = m_z + static_cast<i32>(z);

        if (x > 0 && ShouldAddFace(Face::LEFT, v, m_voxels[INDEX(x-1, y, z)]))                AddFace(Face::LEFT, ax, ay, az, v, y_height);
        if (x < CHUNK_WIDTH - 1 && ShouldAddFace(Face::RIGHT, v, m_voxels[INDEX(x+1, y, z)])) AddFace(Face::RIGHT, ax, ay, az, v, y_height);
        if (y > 0 && ShouldAddFace(Face::BOTTOM, v, m_voxels[INDEX(x, y-1, z)]))              AddFace(Face::BOTTOM, ax, ay, az, v, y_height);
        if (should_add_top_face)                                                              AddFace(Face::TOP, ax, ay, az, v, y_height);
        if (z > 0 && ShouldAddFace(Face::BACK, v, m_voxels[INDEX(x, y, z-1)]))                AddFace(Face::BACK, ax, ay, az, v, y_height);
        if (z < CHUNK_WIDTH - 1 && ShouldAddFace(Face::FRONT, v, m_voxels[INDEX(x, y, z+1)])) AddFace(Face::FRONT, ax, ay, az, v, y_height);
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

                float y_height = 1.0f;
                bool should_add_top_face = y < CHUNK_HEIGHT - 1 && ShouldAddFace(Face::TOP, v, m_voxels[INDEX(0, y+1, z)]);
                if (v.type == Voxel::Type::WATER && should_add_top_face) y_height = 1.0f - WATER_HEIGHT_OFFSET; 

                i32 ax = m_x;
                i32 ay = static_cast<i32>(y);
                i32 az = m_z + static_cast<i32>(z);
                if (ShouldAddFace(Face::LEFT, v, neighbor->m_voxels[INDEX(CHUNK_WIDTH - 1, y, z)])) AddFace(Face::LEFT, ax, ay, az, v, y_height);
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

                float y_height = 1.0f;
                bool should_add_top_face = y < CHUNK_HEIGHT - 1 && ShouldAddFace(Face::TOP, v, m_voxels[INDEX(CHUNK_WIDTH-1, y+1, z)]);
                if (v.type == Voxel::Type::WATER && should_add_top_face) y_height = 1.0f - WATER_HEIGHT_OFFSET; 

                i32 ax = m_x + CHUNK_WIDTH - 1;
                i32 ay = static_cast<i32>(y);
                i32 az = m_z + static_cast<i32>(z);
                if (ShouldAddFace(Face::RIGHT, v, neighbor->m_voxels[INDEX(0, y, z)])) AddFace(Face::RIGHT, ax, ay, az, v, y_height);
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

                float y_height = 1.0f;
                bool should_add_top_face = y < CHUNK_HEIGHT - 1 && ShouldAddFace(Face::TOP, v, m_voxels[INDEX(x, y+1, 0)]);
                if (v.type == Voxel::Type::WATER && should_add_top_face) y_height = 1.0f - WATER_HEIGHT_OFFSET; 

                i32 ax = m_x + static_cast<i32>(x);
                i32 ay = static_cast<i32>(y);
                i32 az = m_z;
                if (ShouldAddFace(Face::BACK, v, neighbor->m_voxels[INDEX(x, y, CHUNK_WIDTH - 1)])) AddFace(Face::BACK, ax, ay, az, v, y_height);
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

                float y_height = 1.0f;
                bool should_add_top_face = y < CHUNK_HEIGHT - 1 && ShouldAddFace(Face::TOP, v, m_voxels[INDEX(x, y+1, CHUNK_WIDTH-1)]);
                if (v.type == Voxel::Type::WATER && should_add_top_face) y_height = 1.0f - WATER_HEIGHT_OFFSET; 

                i32 ax = m_x + static_cast<i32>(x);
                i32 ay = static_cast<i32>(y);
                i32 az = m_z + CHUNK_WIDTH - 1;
                if (ShouldAddFace(Face::FRONT, v, neighbor->m_voxels[INDEX(x, y, 0)])) AddFace(Face::FRONT, ax, ay, az, v, y_height);
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
    m_water_mesh.Load();
    m_loaded = true;
}

void Chunk::RenderSolid() { m_solid_mesh.Render(); }
void Chunk::RenderTransparent() { m_transparent_mesh.Render(); }
void Chunk::RenderWater() { m_water_mesh.Render(); }