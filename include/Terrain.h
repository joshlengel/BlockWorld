#pragma once

#include"Chunk.h"

#include<vector>
#include<cstdint>

class Noise;
class BlockDB;

class TerrainGenerator
{
public:
    virtual ~TerrainGenerator() = default;

    virtual void Generate(Chunk &chunk) const = 0;
};

class BiomeType
{
public:
    virtual ~BiomeType() = default;

    virtual float GetStrength(float humidity, float temperature) const = 0;
    virtual uint16_t GetHeight(int32_t x, int32_t z) const = 0;
    virtual Voxel Generate(uint16_t height, uint16_t y) const = 0;
};

class BiomeGenerator : public TerrainGenerator
{
public:
    BiomeGenerator(const Noise &noise, BlockDB &db);
    ~BiomeGenerator();

    virtual void Generate(Chunk &chunk) const override;

private:
    const Noise &m_noise;
    BlockDB &m_db;

    std::vector<BiomeType*> m_biomes;

    uint16_t GetPlainsHeight(int32_t x, int32_t z) const;
    uint16_t GetMountainsHeight(int32_t x, int32_t z) const;
    uint16_t GetDesertHeight(int32_t x, int32_t z) const;

    void GetBiomeParams(int32_t x, int32_t z, float &humidity, float &temperature) const;
    BiomeType *GetBiome(float humidity, float temperature) const;
};