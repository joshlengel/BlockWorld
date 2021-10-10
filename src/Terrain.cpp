#include"Terrain.h"
#include"Noise.h"
#include"Chunk.h"

PlainsGenerator::PlainsGenerator(const Noise &noise):
    m_noise(noise)
{}

void PlainsGenerator::Generate(Chunk &chunk) const
{
    for (size_t x = 0; x < CHUNK_WIDTH; ++x)
    for (size_t z = 0; z < CHUNK_WIDTH; ++z)
    {
        uint32_t height = m_noise.Sample(chunk.GetX() + static_cast<int32_t>(x), chunk.GetZ() + static_cast<int32_t>(z), 27, 7, 0.03f);

        for (size_t y = 0; y < height - 3; ++y) chunk.SetBlock(x, y, z, { Voxel::Type::STONE });
        for (size_t y = height - 3; y < height; ++y) chunk.SetBlock(x, y, z, { Voxel::Type::DIRT });
        chunk.SetBlock(x, height, z, { Voxel::Type::GRASS });
    }
}