#pragma once

#include"Chunk.h"

#include<set>
#include<cstdint>
#include<random>
#include<mutex>

class Noise;
class BlockDB;

class TerrainGenerator
{
public:
    virtual ~TerrainGenerator() = default;

    virtual void Generate(Chunk &chunk) = 0;
};

class BiomeType
{
public:
    enum class Type
    {
        PLAINS,
        MOUNTAINS,
        DESERT
    };

    virtual ~BiomeType() = default;

    virtual Type GetType() const = 0;
    virtual float GetStrength(float humidity, float temperature) const = 0;
    virtual uint16_t GetHeight(int32_t x, int32_t z) const = 0;
    virtual Voxel Generate(uint16_t height, uint16_t y) const = 0;
};

class BiomeGenerator : public TerrainGenerator
{
public:
    BiomeGenerator(uint_fast32_t seed, BlockDB &db);
    ~BiomeGenerator();

    virtual void Generate(Chunk &chunk) override;

private:
    std::mt19937 m_seed_gen;
    const Noise &m_noise;
    BlockDB &m_db;

    std::vector<BiomeType*> m_biomes;
    std::vector<std::pair<Vec3i, Voxel>> m_queued_blocks;

    std::mutex m_mutex;

    uint16_t GetPlainsHeight(int32_t x, int32_t z) const;
    uint16_t GetMountainsHeight(int32_t x, int32_t z) const;
    uint16_t GetDesertHeight(int32_t x, int32_t z) const;

    void GetBiomeParams(int32_t x, int32_t z, float &humidity, float &temperature) const;
    BiomeType *GetBiome(float humidity, float temperature) const;
    bool HasTree(int32_t x, int32_t z) const;
    void QueueBlock(Chunk &chunk, int32_t x, uint16_t y, int32_t z, const Voxel &v);
};