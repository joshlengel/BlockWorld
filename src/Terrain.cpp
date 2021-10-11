#include"Terrain.h"
#include"Noise.h"
#include"Chunk.h"
#include"Database.h"

PlainsGenerator::PlainsGenerator(const Noise &noise, BlockDB &db):
    m_noise(noise),
    m_db(db)
{}

void PlainsGenerator::Generate(Chunk &chunk) const
{
    for (uint16_t x = 0; x < CHUNK_WIDTH; ++x)
    for (uint16_t z = 0; z < CHUNK_WIDTH; ++z)
    {
        uint16_t height = m_noise.Sample(chunk.GetX() + static_cast<int32_t>(x), chunk.GetZ() + static_cast<int32_t>(z), 27, 7, 0.03f);

        for (uint16_t y = 0; y < height - 3; ++y) chunk.SetBlock({ x, y, z }, { Voxel::Type::STONE });
        for (uint16_t y = height - 3; y < height; ++y) chunk.SetBlock({ x, y, z }, { Voxel::Type::DIRT });
        chunk.SetBlock({ x, height, z }, { Voxel::Type::GRASS });
    }

    m_db.QueryBlocks(chunk.GetX(), chunk.GetZ(), [&](const Vec3ui16 &pos, const Voxel &v){ chunk.SetBlock(pos, v); });
}