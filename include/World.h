#pragma once

#include"Terrain.h"
#include"Noise.h"

#include<cstdint>
#include<mutex>
#include<condition_variable>
#include<vector>

#define RENDER_DISTANCE 5

class Chunk;
class Vec3;

class World
{
public:
    World(int32_t x, int32_t z, uint_fast32_t seed);
    ~World();

    void Update(const Vec3 &camera_pos);
    void Render();

private:
    std::vector<Chunk*> m_chunks;
    std::mutex m_loader_mutex;
    std::condition_variable m_loader_cv;
    std::vector<std::pair<int32_t, int32_t>> m_load_queue;
    bool m_running;

    Noise m_noise;
    PlainsGenerator m_generator;
};