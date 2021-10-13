#pragma once

#include"Core.h"
#include"Chunk.h"

#include<random>
#include<mutex>

#define WATER_HEIGHT 25

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
    virtual ui32 GetHeight(i32 x, i32 z) const = 0;
    virtual Voxel Generate(ui32 height, ui32 y) const = 0;
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

    ui32 GetPlainsHeight(i32 x, i32 z) const;
    ui32 GetMountainsHeight(i32 x, i32 z) const;
    ui32 GetDesertHeight(i32 x, i32 z) const;

    void GetBiomeParams(i32 x, i32 z, float &humidity, float &temperature) const;
    BiomeType *GetBiome(float humidity, float temperature) const;
    bool HasTree(i32 x, i32 z) const;
    void QueueBlock(Chunk &chunk, i32 x, ui32 y, i32 z, const Voxel &v);
};