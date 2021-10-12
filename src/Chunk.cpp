#include"Chunk.h"

#include<cstddef>
#include<cstring>
#include<stdexcept>

#define INDEX(x,y,z) ((y) * CHUNK_WIDTH * CHUNK_WIDTH + (z) * CHUNK_WIDTH + (x))

static const uint16_t VOXEL_INDICES[] =
{
    0, 0, 0,
    1, 1, 1,
    0, 1, 2,
    3, 3, 3,
    4, 4, 4
};

uint16_t Voxel::GetSideIndex() const { return VOXEL_INDICES[3 * static_cast<uint16_t>(type)]; }
uint16_t Voxel::GetBottomIndex() const { return VOXEL_INDICES[3 * static_cast<uint16_t>(type) + 1]; }
uint16_t Voxel::GetTopIndex() const { return VOXEL_INDICES[3 * static_cast<uint16_t>(type) + 2]; }

Chunk::Chunk(int32_t x, int32_t z):
    m_x(x), m_z(z),
    m_loaded(false)
{
    std::memset(m_voxels, 0, sizeof(m_voxels));
}

int32_t Chunk::GetX() const { return m_x; }
int32_t Chunk::GetZ() const { return m_z; }
bool Chunk::Loaded() const { return m_loaded; }

Voxel &Chunk::GetBlock(const Vec3ui16 &pos) { return m_voxels[INDEX(pos.x,pos.y,pos.z)]; }
void Chunk::SetBlock(const Vec3ui16 &pos, const Voxel &v) { m_voxels[INDEX(pos.x,pos.y,pos.z)] = v; }

void Chunk::GenerateMesh()
{
    m_mesh.Flush();
    for (uint16_t x = 0; x < CHUNK_WIDTH; ++x)
    for (uint16_t y = 0; y < CHUNK_HEIGHT; ++y)
    for (uint16_t z = 0; z < CHUNK_WIDTH; ++z)
    {
        Voxel &v = m_voxels[INDEX(x, y, z)];
        if (v.type == Voxel::Type::AIR) continue;

        int32_t ax = m_x + static_cast<int32_t>(x);
        int32_t ay = static_cast<int32_t>(y);
        int32_t az = m_z + static_cast<int32_t>(z);

        if (x > 0 && m_voxels[INDEX(x-1, y, z)].type == Voxel::Type::AIR)                m_mesh.AddFace(Face::LEFT, v.GetSideIndex(), ax, ay, az);
        if (x < CHUNK_WIDTH - 1 && m_voxels[INDEX(x+1, y, z)].type == Voxel::Type::AIR)  m_mesh.AddFace(Face::RIGHT, v.GetSideIndex(), ax, ay, az);
        if (y > 0 && m_voxels[INDEX(x, y-1, z)].type == Voxel::Type::AIR)                m_mesh.AddFace(Face::BOTTOM, v.GetBottomIndex(), ax, ay, az);
        if (y < CHUNK_HEIGHT - 1 && m_voxels[INDEX(x, y+1, z)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::TOP, v.GetTopIndex(), ax, ay, az);
        if (z > 0 && m_voxels[INDEX(x, y, z-1)].type == Voxel::Type::AIR)                m_mesh.AddFace(Face::BACK, v.GetSideIndex(), ax, ay, az);
        if (z < CHUNK_WIDTH - 1 && m_voxels[INDEX(x, y, z+1)].type == Voxel::Type::AIR)  m_mesh.AddFace(Face::FRONT, v.GetSideIndex(), ax, ay, az);
    }
    m_loaded = false;
}

void Chunk::GenerateBorder(Face face, Chunk *neighbor)
{
    switch (face)
    {
        case Face::LEFT:
        {
            for (uint16_t y = 0; y < CHUNK_HEIGHT; ++y)
            for (uint16_t z = 0; z < CHUNK_WIDTH; ++z)
            {
                Voxel &v = m_voxels[INDEX(0, y, z)];
                if (v.type == Voxel::Type::AIR) continue;

                int32_t ax = m_x;
                int32_t ay = static_cast<int32_t>(y);
                int32_t az = m_z + static_cast<int32_t>(z);
                if (neighbor->m_voxels[INDEX(CHUNK_WIDTH - 1, y, z)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::LEFT, v.GetSideIndex(), ax, ay, az);
            }
            break;
        }

        case Face::RIGHT:
        {
            for (uint16_t y = 0; y < CHUNK_HEIGHT; ++y)
            for (uint16_t z = 0; z < CHUNK_WIDTH; ++z)
            {
                Voxel &v = m_voxels[INDEX(CHUNK_WIDTH - 1, y, z)];
                if (v.type == Voxel::Type::AIR) continue;

                int32_t ax = m_x + CHUNK_WIDTH - 1;
                int32_t ay = static_cast<int32_t>(y);
                int32_t az = m_z + static_cast<int32_t>(z);
                if (neighbor->m_voxels[INDEX(0, y, z)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::RIGHT, v.GetSideIndex(), ax, ay, az);
            }
            break;
        }

        case Face::BACK:
        {
            for (uint16_t x = 0; x < CHUNK_WIDTH; ++x)
            for (uint16_t y = 0; y < CHUNK_HEIGHT; ++y)
            {
                Voxel &v = m_voxels[INDEX(x, y, 0)];
                if (v.type == Voxel::Type::AIR) continue;

                int32_t ax = m_x + static_cast<int32_t>(x);
                int32_t ay = static_cast<int32_t>(y);
                int32_t az = m_z;
                if (neighbor->m_voxels[INDEX(x, y, CHUNK_WIDTH - 1)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::BACK, v.GetSideIndex(), ax, ay, az);
            }
            break;
        }

        case Face::FRONT:
        {
            for (uint16_t x = 0; x < CHUNK_WIDTH; ++x)
            for (uint16_t y = 0; y < CHUNK_HEIGHT; ++y)
            {
                Voxel &v = m_voxels[INDEX(x, y, CHUNK_WIDTH - 1)];
                if (v.type == Voxel::Type::AIR) continue;

                int32_t ax = m_x + static_cast<int32_t>(x);
                int32_t ay = static_cast<int32_t>(y);
                int32_t az = m_z + CHUNK_WIDTH - 1;
                if (neighbor->m_voxels[INDEX(x, y, 0)].type == Voxel::Type::AIR) m_mesh.AddFace(Face::FRONT, v.GetSideIndex(), ax, ay, az);
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