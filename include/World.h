#pragma once

#include"Terrain.h"
#include"Noise.h"
#include"Chunk.h"

#include<cstdint>
#include<mutex>
#include<condition_variable>
#include<vector>

#define RENDER_DISTANCE 8

class Vec3;

class World
{
public:
    World(int32_t x, int32_t z, uint_fast32_t seed);
    ~World();

    void Break(const Vec3 &ray_start, const Vec3 &ray_direction);
    void Place(const Vec3 &ray_start, const Vec3 &ray_direction, Voxel::Type type);

    void Update(const Vec3 &camera_pos);
    void Render();

private:
    std::vector<Chunk*> m_chunks;
    std::mutex m_create_mutex, m_generate_mutex, m_chunk_mutex;
    std::condition_variable m_create_cv, m_generate_cv;
    std::vector<std::pair<int32_t, int32_t>> m_create_queue;
    std::vector<Chunk*> m_generate_queue;
    bool m_running;

    Noise m_noise;
    PlainsGenerator m_generator;

    void DoRaycast(const Vec3 &ray_start, const Vec3 &ray_direction, Chunk **previous_chunk, Chunk **hit_chunk, Voxel **previous, Voxel **hit);
};