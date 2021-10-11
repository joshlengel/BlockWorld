#pragma once

#include<cstdint>

class Chunk;
class Noise;
class BlockDB;

class TerrainGenerator
{
public:
    virtual ~TerrainGenerator() = default;

    virtual void Generate(Chunk &chunk) const = 0;
};

class PlainsGenerator : public TerrainGenerator
{
public:
    PlainsGenerator(const Noise &noise, BlockDB &db);

    virtual void Generate(Chunk &chunk) const override;

private:
    const Noise &m_noise;
    BlockDB &m_db;
};