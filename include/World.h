#pragma once

#include"Terrain.h"
#include"Noise.h"
#include"Chunk.h"
#include"Vec.h"

#include<cstdint>
#include<mutex>
#include<condition_variable>
#include<vector>

#define RENDER_DISTANCE 8

class BlockDB;

class World
{
public:
    World(int32_t x, int32_t z, BlockDB &db, uint_fast32_t seed);
    ~World();

    void Break(const Vec3f &ray_start, const Vec3f &ray_direction);
    void Place(const Vec3f &ray_start, const Vec3f &ray_direction, Voxel::Type type);

    void Update(const Vec3f &camera_pos);
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

    BlockDB &m_db;

    bool DoRaycast(const Vec3f &ray_start, const Vec3f &ray_direction, Chunk **previous_chunk, Chunk **hit_chunk, Vec3ui16 &previous, Vec3ui16 &hit);
};