#pragma once

#include"Vec.h"

#include<cstdint>

#define NOISE_GRID_SIZE 256

class Noise
{
public:
    Noise(uint_fast32_t seed);

    float Sample(float x, float y, float center, float amplitude, float frequency) const;

private:
    Vec2 m_grid[NOISE_GRID_SIZE * NOISE_GRID_SIZE];
};