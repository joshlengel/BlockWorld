#include"Noise.h"

#include<random>

#define INDEX(x,y) ((y) * NOISE_GRID_SIZE + (x))

static uint32_t Mod(float a, float b)
{
    return static_cast<uint32_t>(a - std::floor(a / b) * b);
}

static float Smoothstep(float x)
{
    return x * x * x * (x * (x * 6 - 15) + 10);
}

static float Lerp(float x, float a, float b)
{
    return x * (b - a) + a;
}

Noise::Noise(uint_fast32_t seed)
{
    std::mt19937 rand(seed);
    std::uniform_real_distribution<float> dist(0, static_cast<float>(M_PI * 2));

    for (size_t i = 0; i < NOISE_GRID_SIZE * NOISE_GRID_SIZE; ++i)
    {
        float angle = dist(rand);
        m_grid[i] = { std::cos(angle), std::sin(angle) };
    }
}

float Noise::Sample(float x, float y, float center, float amplitude, float frequency) const
{
    float sx = x * frequency;
    float sy = y * frequency;

    Vec2f s = { sx, sy };

    uint32_t gx0 = Mod(std::floor(sx), NOISE_GRID_SIZE);
    uint32_t gy0 = Mod(std::floor(sy), NOISE_GRID_SIZE);

    uint32_t gx1 = gx0 == NOISE_GRID_SIZE - 1? 0 : gx0 + 1;
    uint32_t gy1 = gy0 == NOISE_GRID_SIZE - 1? 0 : gy0 + 1;

    float cx0 = std::floor(sx);
    float cy0 = std::floor(sy);

    float cx1 = cx0 + 1;
    float cy1 = cy0 + 1;

    float d1 = Vec2f::Dot(m_grid[INDEX(gx0,gy0)], s - Vec2f(cx0, cy0));
    float d2 = Vec2f::Dot(m_grid[INDEX(gx1,gy0)], s - Vec2f(cx1, cy0));
    float d3 = Vec2f::Dot(m_grid[INDEX(gx0,gy1)], s - Vec2f(cx0, cy1));
    float d4 = Vec2f::Dot(m_grid[INDEX(gx1,gy1)], s - Vec2f(cx1, cy1));

    float i1 = Lerp(Smoothstep(sx - cx0), d1, d2);
    float i2 = Lerp(Smoothstep(sx - cx0), d3, d4);

    float res = Lerp(Smoothstep(sy - cy0), i1, i2);
    return res * amplitude + center;
}