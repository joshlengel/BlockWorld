#include"Terrain.h"
#include"Noise.h"
#include"Chunk.h"
#include"Database.h"

#include<cmath>
#include<algorithm>

#define BIOME_LERP_RADIUS 3
#define BIOME_STRUCTURE_BUFFER_RADIUS 1
#define TOTAL_CHUNK_WIDTH (CHUNK_WIDTH + 2 * (BIOME_LERP_RADIUS + BIOME_STRUCTURE_BUFFER_RADIUS))
#define CHUNK_BUFFER_WIDTH (CHUNK_WIDTH + 2 * BIOME_STRUCTURE_BUFFER_RADIUS)
#define TREE_CHANCE 0.01f

#define INDEX(x,z,w) ((z) * (w) + (x))

class PlanesBiome : public BiomeType
{
public:
    PlanesBiome(const Noise &noise): m_noise(noise) {}

    virtual Type GetType() const override { return Type::PLAINS; }

    virtual float GetStrength(float humidity, float temperature) const override
    {
        float dh = humidity - 0.4f;
        float dt = temperature - 0.5f;
        return 1.0f / (dh * dh + dt * dt);
    }

    virtual ui32 GetHeight(i32 x, i32 z) const override
    {
        return static_cast<ui32>(m_noise.Sample(x - 985232, z + 852837, 27, 7, 0.03f));
    }

    virtual Voxel Generate(ui32 height, ui32 y) const override
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

    virtual Type GetType() const override { return Type::MOUNTAINS; }

    virtual float GetStrength(float humidity, float temperature) const override
    {
        float dh = humidity - 0.3f;
        float dt = temperature - 0.3f;
        return 1.0f / (dh * dh + dt * dt);
    }

    virtual ui32 GetHeight(i32 x, i32 z) const override
    {
        float o1 = m_noise.Sample(x + 4830394, z - 328578, 40, 15, 0.04f);
        float o2 = m_noise.Sample(x - 348747,  z - 871983, 0, 3, 0.07f);
        float o3 = m_noise.Sample(x + 1764938, z + 42876,  1, 1, 0.1f);

        return static_cast<ui32>(o1 + o2 + o3);
    }

    virtual Voxel Generate(ui32 height, ui32 y) const override
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

    virtual Type GetType() const override { return Type::DESERT; }

    virtual float GetStrength(float humidity, float temperature) const override
    {
        float dh = humidity - 0.5f;
        float dt = temperature - 0.7f;
        return 1.0f / (dh * dh + dt * dt);
    }

    virtual ui32 GetHeight(i32 x, i32 z) const override
    {
        float o1 = m_noise.Sample(x - 837492,  z - 394398, 30, 8, 0.02f);
        float o2 = m_noise.Sample(x + 234928, z - 7216473, 0, 15, 0.05f);

        return static_cast<ui32>(o1 + o2);
    }

    virtual Voxel Generate(ui32 height, ui32 y) const override
    {
        if (y < height - 6) return { Voxel::Type::STONE };
        else if (y <= height) return { Voxel::Type::SAND };
        else return { Voxel::Type::AIR };
    }

private:
    const Noise &m_noise;
};

BiomeGenerator::BiomeGenerator(uint_fast32_t seed, BlockDB &db):
    m_seed_gen(seed),
    m_noise(std::uniform_int_distribution<uint_fast32_t>()(m_seed_gen)),
    m_db(db),
    m_biomes(),
    m_queued_blocks()
{
    m_biomes.push_back(new PlanesBiome(m_noise));
    m_biomes.push_back(new MountainsBiome(m_noise));
    m_biomes.push_back(new DesertBiome(m_noise));
}

BiomeGenerator::~BiomeGenerator()
{
    for (BiomeType *biome : m_biomes) delete biome;
}

void BiomeGenerator::GetBiomeParams(i32 x, i32 z, float &humidity, float &temperature) const
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

bool BiomeGenerator::HasTree(i32 x, i32 z) const
{
    std::mt19937 rand;
    rand.seed(static_cast<uint_fast32_t>(m_noise.Sample(x + 8479832, z - 200983, 500.0f, 500.0f, 0.001f) * 8374982.0f));
    return std::uniform_real_distribution<float>()(rand) < TREE_CHANCE;
}

void BiomeGenerator::QueueBlock(Chunk &chunk, i32 x, ui32 y, i32 z, const Voxel &v)
{
    if (x < chunk.GetX() || x >= chunk.GetX() + CHUNK_WIDTH || z < chunk.GetZ() || z >= chunk.GetZ() + CHUNK_WIDTH) m_queued_blocks.push_back({ { x, static_cast<i32>(y), z }, v });
    else
    {
        Vec3ui pos = { static_cast<ui32>(x - chunk.GetX()), y, static_cast<ui32>(z - chunk.GetZ()) };
        Voxel &prev = chunk.GetBlock(pos);
        if (prev.type == Voxel::Type::AIR || prev.type == Voxel::Type::LEAVES) chunk.SetBlock(pos, v);
    }
}

void BiomeGenerator::Generate(Chunk &chunk)
{
    ui32 heights[TOTAL_CHUNK_WIDTH * TOTAL_CHUNK_WIDTH];
    BiomeType *biomes[TOTAL_CHUNK_WIDTH * TOTAL_CHUNK_WIDTH];

    // Get heights and biomes
    for (ui32 x = 0; x < TOTAL_CHUNK_WIDTH; ++x)
    for (ui32 z = 0; z < TOTAL_CHUNK_WIDTH; ++z)
    {
        i32 ax = chunk.GetX() + static_cast<i32>(x) - BIOME_LERP_RADIUS - BIOME_STRUCTURE_BUFFER_RADIUS;
        i32 az = chunk.GetZ() + static_cast<i32>(z) - BIOME_LERP_RADIUS - BIOME_STRUCTURE_BUFFER_RADIUS;

        float humidity, temperature;
        GetBiomeParams(ax, az, humidity, temperature);
        BiomeType *biome = GetBiome(humidity, temperature);
        heights[INDEX(x,z,TOTAL_CHUNK_WIDTH)] = biome->GetHeight(ax, az);
        biomes[INDEX(x,z,TOTAL_CHUNK_WIDTH)] = biome;
    }

    float lerp_heights[CHUNK_BUFFER_WIDTH * CHUNK_BUFFER_WIDTH];

    // Do biome height interpolation
    for (ui32 x = 0; x < CHUNK_BUFFER_WIDTH; ++x)
    for (ui32 z = 0; z < CHUNK_BUFFER_WIDTH; ++z)
    {
        float sum_height = 0.0f;
        float sum_weight = 0.0f;
        float other_biome = 0.0f;

        ui32 height = heights[INDEX(x + BIOME_LERP_RADIUS, z + BIOME_LERP_RADIUS, TOTAL_CHUNK_WIDTH)];
        BiomeType *biome = biomes[INDEX(x + BIOME_LERP_RADIUS, z + BIOME_LERP_RADIUS, TOTAL_CHUNK_WIDTH)];

        for (i32 ix = -BIOME_LERP_RADIUS; ix <= BIOME_LERP_RADIUS; ++ix)
        for (i32 iz = -BIOME_LERP_RADIUS; iz <= BIOME_LERP_RADIUS; ++iz)
        {
            BiomeType *other = biomes[INDEX(x + BIOME_LERP_RADIUS - ix, z + BIOME_LERP_RADIUS - iz, TOTAL_CHUNK_WIDTH)];
            
            float weight = 1.0f / std::max(static_cast<float>(ix * ix + iz * iz), 1.0f);
            if (other != biome) other_biome += weight;

            sum_height += static_cast<float>(heights[INDEX(x + BIOME_LERP_RADIUS - ix, z + BIOME_LERP_RADIUS - iz, TOTAL_CHUNK_WIDTH)]) * weight;
            sum_weight += weight;
        }

        lerp_heights[INDEX(x, z, CHUNK_BUFFER_WIDTH)] = other_biome > 0.0f? static_cast<ui32>(sum_height / sum_weight) : height;
    }

    // Generate terrain blocks
    for (ui32 x = 0; x < CHUNK_WIDTH; ++x)
    for (ui32 z = 0; z < CHUNK_WIDTH; ++z)
    {
        ui32 height = lerp_heights[INDEX(x + BIOME_STRUCTURE_BUFFER_RADIUS, z + BIOME_STRUCTURE_BUFFER_RADIUS, CHUNK_BUFFER_WIDTH)];
        BiomeType *biome = biomes[INDEX(x + BIOME_STRUCTURE_BUFFER_RADIUS + BIOME_LERP_RADIUS, z + BIOME_STRUCTURE_BUFFER_RADIUS + BIOME_LERP_RADIUS, TOTAL_CHUNK_WIDTH)];
        for (ui32 y = 0; y <= height; ++y) chunk.SetBlock({ x, y, z }, biome->Generate(height, y));
    }

    // Generate structures
    for (ui32 x = 0; x < CHUNK_BUFFER_WIDTH; ++x)
    for (ui32 z = 0; z < CHUNK_BUFFER_WIDTH; ++z)
    {
        i32 ax = chunk.GetX() + x - BIOME_STRUCTURE_BUFFER_RADIUS;
        i32 az = chunk.GetZ() + z - BIOME_STRUCTURE_BUFFER_RADIUS;

        ui32 height = lerp_heights[INDEX(x, z, CHUNK_BUFFER_WIDTH)];
        BiomeType *biome = biomes[INDEX(x + BIOME_LERP_RADIUS, z + BIOME_LERP_RADIUS, TOTAL_CHUNK_WIDTH)];

        if (biome->GetType() == BiomeType::Type::PLAINS)
        {
            // Generate tree
            if (HasTree(ax, az))
            {
                Voxel log{ Voxel::Type::LOG };
                Voxel leaves{ Voxel::Type::LEAVES };

                std::unique_lock<std::mutex> lock(m_mutex);
                QueueBlock(chunk, ax, height + 1, az, log);
                QueueBlock(chunk, ax, height + 2, az, log);
                QueueBlock(chunk, ax, height + 3, az, log);
                QueueBlock(chunk, ax, height + 4, az, log);
                QueueBlock(chunk, ax, height + 5, az, leaves);

                QueueBlock(chunk, ax - 1, height + 3, az, leaves);
                QueueBlock(chunk, ax - 1, height + 4, az, leaves);
                QueueBlock(chunk, ax - 1, height + 5, az, leaves);
                QueueBlock(chunk, ax + 1, height + 3, az, leaves);
                QueueBlock(chunk, ax + 1, height + 4, az, leaves);
                QueueBlock(chunk, ax + 1, height + 5, az, leaves);
                QueueBlock(chunk, ax, height + 3, az - 1, leaves);
                QueueBlock(chunk, ax, height + 4, az - 1, leaves);
                QueueBlock(chunk, ax, height + 5, az - 1, leaves);
                QueueBlock(chunk, ax, height + 3, az + 1, leaves);
                QueueBlock(chunk, ax, height + 4, az + 1, leaves);
                QueueBlock(chunk, ax, height + 5, az + 1, leaves);
                QueueBlock(chunk, ax - 1, height + 3, az - 1, leaves);
                QueueBlock(chunk, ax - 1, height + 4, az - 1, leaves);
                QueueBlock(chunk, ax + 1, height + 3, az - 1, leaves);
                QueueBlock(chunk, ax + 1, height + 4, az - 1, leaves);
                QueueBlock(chunk, ax - 1, height + 3, az + 1, leaves);
                QueueBlock(chunk, ax - 1, height + 4, az + 1, leaves);
                QueueBlock(chunk, ax + 1, height + 3, az + 1, leaves);
                QueueBlock(chunk, ax + 1, height + 4, az + 1, leaves);
            }
        }
    }

    // Get structures generated by other chunks
    std::unique_lock<std::mutex> lock(m_mutex);
    size_t index = 0;
    while (index < m_queued_blocks.size())
    {
        auto itr = m_queued_blocks.begin() + index;
        const auto &block = *itr;

        if (block.first.x < chunk.GetX() || block.first.x >= chunk.GetX() + CHUNK_WIDTH || block.first.z < chunk.GetZ() || block.first.z >= chunk.GetZ() + CHUNK_WIDTH)
        {
            ++index;
        }
        else
        {
            Vec3ui pos =
            {
                static_cast<ui32>(block.first.x - chunk.GetX()),
                static_cast<ui32>(block.first.y),
                static_cast<ui32>(block.first.z - chunk.GetZ())
            };
            Voxel &prev = chunk.GetBlock(pos);

            if (prev.type == Voxel::Type::AIR || prev.type == Voxel::Type::LEAVES) chunk.SetBlock(pos, block.second);

            m_queued_blocks.erase(itr);
        }
    }
    lock.unlock();

    // Generate broken and placed blocks
    m_db.QueryBlocks(chunk.GetX(), chunk.GetZ(), [&](const Vec3ui &pos, const Voxel &v){ chunk.SetBlock(pos, v); });
}