#pragma once

#include"Terrain.h"
#include"Noise.h"
#include"Chunk.h"
#include"Loader.h"
#include"Vec.h"

#include<cstdint>
#include<functional>

#define RENDER_DISTANCE 8

class BlockDB;

class World
{
public:
    World(BlockDB &db, uint_fast32_t seed);
    ~World();

    void Break(const Vec3f &ray_start, const Vec3f &ray_direction);
    void Place(const Vec3f &ray_start, const Vec3f &ray_direction, Voxel::Type type, const std::function<bool(const Vec3f&)> &should_place);

    Voxel *GetBlock(const Vec3f &position);

    void Update(const Vec3f &camera_pos);
    void Render();

private:
    Loader m_loader;

    Noise m_noise;
    BiomeGenerator m_generator;

    BlockDB &m_db;

    bool DoRaycast(const Vec3f &ray_start, const Vec3f &ray_direction, Chunk **previous_chunk, Chunk **hit_chunk, Vec3ui16 &previous, Vec3ui16 &hit);
};