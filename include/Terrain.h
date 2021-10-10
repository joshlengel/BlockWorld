#pragma once

#include<cstdint>

class Chunk;
class Noise;

class TerrainGenerator
{
public:
    virtual ~TerrainGenerator() = default;

    virtual void Generate(Chunk &chunk) const = 0;
};

class PlainsGenerator : public TerrainGenerator
{
public:
    PlainsGenerator(const Noise &noise);

    virtual void Generate(Chunk &chunk) const override;

private:
    const Noise &m_noise;
};