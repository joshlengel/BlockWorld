#include"Terrain.h"
#include"Noise.h"
#include"Chunk.h"
#include"Database.h"

#include<cmath>
#include<algorithm>

#define BIOME_LERP_RADIUS 3

class PlanesBiome : public BiomeType
{
public:
    PlanesBiome(const Noise &noise): m_noise(noise) {}

    virtual float GetStrength(float humidity, float temperature) const override
    {
        float dh = humidity - 0.4f;
        float dt = temperature - 0.5f;
        return 1.0f / (dh * dh + dt * dt);
    }

    virtual uint16_t GetHeight(int32_t x, int32_t z) const override
    {
        return static_cast<uint16_t>(m_noise.Sample(x - 985232, z + 852837, 27, 7, 0.03f));
    }

    virtual Voxel Generate(uint16_t height, uint16_t y) const override
    {
        if (y < height - 3) return { Voxel::Type::STONE };
        else if (y < height) return { Voxel::Type::DIRT };
        else if (y == height) return { Voxel::Type::GRASS };
        else return { Voxel::Type::AIR };
    }

private:
    const Noise &m_noise;
};

class MountainsBiome : public BiomeType
{
public:
    MountainsBiome(const Noise &noise): m_noise(noise) {}

    virtual float GetStrength(float humidity, float temperature) const override
    {
        float dh = humidity - 0.3f;
        float dt = temperature - 0.3f;
        return 1.0f / (dh * dh + dt * dt);
    }

    virtual uint16_t GetHeight(int32_t x, int32_t z) const override
    {
        float o1 = m_noise.Sample(x + 4830394, z - 328578, 40, 15, 0.04f);
        float o2 = m_noise.Sample(x - 348747,  z - 871983, 0, 3, 0.07f);
        float o3 = m_noise.Sample(x + 1764938, z + 42876,  1, 1, 0.1f);

        return static_cast<uint16_t>(o1 + o2 + o3);
    }

    virtual Voxel Generate(uint16_t height, uint16_t y) const override
    {
        if (y <= height) return { Voxel::Type::STONE };
        else return { Voxel::Type::AIR };
    }

private:
    const Noise &m_noise;
};

class DesertBiome : public BiomeType
{
public:
    DesertBiome(const Noise &noise): m_noise(noise) {}

    virtual float GetStrength(float humidity, float temperature) const override
    {
        float dh = humidity - 0.5f;
        float dt = temperature - 0.7f;
        return 1.0f / (dh * dh + dt * dt);
    }

    virtual uint16_t GetHeight(int32_t x, int32_t z) const override
    {
        float o1 = m_noise.Sample(x - 837492,  z - 394398, 30, 8, 0.02f);
        float o2 = m_noise.Sample(x + 234928, z - 7216473, 0, 15, 0.05f);

        return static_cast<uint16_t>(o1 + o2);
    }

    virtual Voxel Generate(uint16_t height, uint16_t y) const override
    {
        if (y < height - 6) return { Voxel::Type::STONE };
        else if (y <= height) return { Voxel::Type::SAND };
        else return { Voxel::Type::AIR };
    }

private:
    const Noise &m_noise;
};

BiomeGenerator::BiomeGenerator(const Noise &noise, BlockDB &db):
    m_noise(noise),
    m_db(db),
    m_biomes()
{
    m_biomes.push_back(new PlanesBiome(noise));
    m_biomes.push_back(new MountainsBiome(noise));
    m_biomes.push_back(new DesertBiome(noise));
}

BiomeGenerator::~BiomeGenerator()
{
    for (BiomeType *biome : m_biomes) delete biome;
}

void BiomeGenerator::GetBiomeParams(int32_t x, int32_t z, float &humidity, float &temperature) const
{
    humidity    = m_noise.Sample(x + 387492, z + 961726, 0.5f, 0.5f, 0.01f);
    temperature = m_noise.Sample(x - 709273, z + 520982, 0.5f, 0.5f, 0.01f);
}

BiomeType *BiomeGenerator::GetBiome(float humidity, float temperature) const
{
    float strength = -INFINITY;
    BiomeType *type = nullptr;
    std::for_each(m_biomes.begin(), m_biomes.end(), [=, &strength, &type](BiomeType *biome) { float s = biome->GetStrength(humidity, temperature); if (s > strength) { strength = s; type = biome; } });
    return type;
}

void BiomeGenerator::Generate(Chunk &chunk) const
{
    for (uint16_t x = 0; x < CHUNK_WIDTH; ++x)
    for (uint16_t z = 0; z < CHUNK_WIDTH; ++z)
    {
        int32_t ax = chunk.GetX() + static_cast<int32_t>(x);
        int32_t az = chunk.GetZ() + static_cast<int32_t>(z);

        float humidity, temperature;
        GetBiomeParams(ax, az, humidity, temperature);
        BiomeType *biome = GetBiome(humidity, temperature);

        float sum_height = 0.0f;
        float sum_weight = 0.0f;
        float other_biome = 0.0f;

        for (int32_t ix = ax - BIOME_LERP_RADIUS; ix <= ax + BIOME_LERP_RADIUS; ++ix)
        for (int32_t iz = az - BIOME_LERP_RADIUS; iz <= az + BIOME_LERP_RADIUS; ++iz)
        {
            float h, t;
            GetBiomeParams(ix, iz, h, t);
            BiomeType *other = GetBiome(h, t);

            float weight = 1.0f / std::max(static_cast<float>((ix - ax) * (ix - ax) + (iz - az) * (iz - az)), 1.0f);
            if (other != biome) other_biome += weight;

            sum_height += static_cast<float>(other->GetHeight(ix, iz)) * weight;
            sum_weight += weight;
        }

        uint16_t height = other_biome > 0.0f? static_cast<uint16_t>(sum_height / sum_weight) : biome->GetHeight(ax, az);

        for (uint16_t y = 0; y <= height; ++y) chunk.SetBlock({ x, y, z }, biome->Generate(height, y));
    }

    m_db.QueryBlocks(chunk.GetX(), chunk.GetZ(), [&](const Vec3ui16 &pos, const Voxel &v){ chunk.SetBlock(pos, v); });
}