#include"World.h"
#include"Chunk.h"
#include"Vec.h"
#include"Database.h"

#include<glad/glad.h>

#include<cmath>
#include<algorithm>
#include<iostream>

World::World(BlockDB &db, uint_fast32_t seed):
    m_loader
    (
        [this](Chunk *chunk)
        {
            m_generator.Generate(*chunk);
        },
        [this](Chunk *chunk)
        {
            chunk->GenerateMesh();

            // Find loaded neighbors
            ChunkInfo *left = m_loader.GetCreatedChunk(chunk->GetX() - CHUNK_WIDTH, chunk->GetZ());
            ChunkInfo *right = m_loader.GetCreatedChunk(chunk->GetX() + CHUNK_WIDTH, chunk->GetZ());
            ChunkInfo *back = m_loader.GetCreatedChunk(chunk->GetX(), chunk->GetZ() - CHUNK_WIDTH);
            ChunkInfo *front = m_loader.GetCreatedChunk(chunk->GetX(), chunk->GetZ() + CHUNK_WIDTH);

            if (left)
            {
                chunk->GenerateBorder(Face::LEFT, left->chunk);
                if (!left->should_generate) left->chunk->GenerateBorder(Face::RIGHT, chunk);
            }
            
            if (right)
            {
                chunk->GenerateBorder(Face::RIGHT, right->chunk);
                if (!right->should_generate) right->chunk->GenerateBorder(Face::LEFT, chunk);
            }
            
            if (back)
            {
                chunk->GenerateBorder(Face::BACK, back->chunk);
                if (!back->should_generate) back->chunk->GenerateBorder(Face::FRONT, chunk);
            }
            
            if (front)
            {
                chunk->GenerateBorder(Face::FRONT, front->chunk);
                if (!front->should_generate) front->chunk->GenerateBorder(Face::BACK, chunk);
            }
        }
    ),
    m_generator(seed, db),
    m_db(db)
{}

World::~World()
{}

Voxel *World::GetBlock(const Vec3f &position)
{
    if (position.y < 0 || position.y >= CHUNK_HEIGHT) return nullptr;

    int32_t cx = static_cast<int32_t>(std::floor(position.x / CHUNK_WIDTH)) * CHUNK_WIDTH;
    int32_t cz = static_cast<int32_t>(std::floor(position.z / CHUNK_WIDTH)) * CHUNK_WIDTH;

    ChunkInfo *info = m_loader.GetLoadedChunk(cx, cz);
    if (info)
    {
        Vec3ui pos =
        {
            static_cast<uint16_t>(position.x - cx),
            static_cast<uint16_t>(position.y),
            static_cast<uint16_t>(position.z - cz)
        };

        return &info->chunk->GetBlock(pos);
    }

    return nullptr;
}

bool World::DoRaycast(const Vec3f &ray_start, const Vec3f &ray_direction, Chunk **previous_chunk, Chunk **hit_chunk, Vec3ui &previous, Vec3ui &hit)
{
    *previous_chunk = *hit_chunk = nullptr;

    size_t iterations = 100;
    float reach = 8.0f;

    float step = reach / iterations;

    for (size_t i = 0; i < iterations; ++i)
    {
        Vec3f position = ray_start + i * step * ray_direction;

        int32_t cx = static_cast<int32_t>(std::floor(position.x / CHUNK_WIDTH)) * CHUNK_WIDTH;
        int32_t cz = static_cast<int32_t>(std::floor(position.z / CHUNK_WIDTH)) * CHUNK_WIDTH;

        ChunkInfo *info = m_loader.GetLoadedChunk(cx, cz);
        if (info)
        {
            Vec3ui pos =
            {
                static_cast<uint16_t>(position.x - static_cast<float>(cx)),
                static_cast<uint16_t>(position.y),
                static_cast<uint16_t>(position.z - static_cast<float>(cz))
            };

            Voxel &voxel = info->chunk->GetBlock(pos);

            previous = hit;
            *previous_chunk = *hit_chunk;

            hit = pos;
            *hit_chunk = info->chunk;

            if (voxel.type != Voxel::Type::AIR && voxel.type != Voxel::Type::WATER) return true;
        }
    }

    *previous_chunk = *hit_chunk = nullptr;
    return false;
}

void World::Break(const Vec3f &ray_start, const Vec3f &ray_direction)
{
    Vec3ui previous, hit;
    Chunk *previous_chunk, *hit_chunk;
    
    if (DoRaycast(ray_start, ray_direction, &previous_chunk, &hit_chunk, previous, hit))
    {
        // Break block and update chunk
        Voxel &v = hit_chunk->GetBlock(hit);
        v.type = Voxel::Type::AIR;
        
        m_db.AddBlock(hit_chunk->GetX(), hit_chunk->GetZ(), hit, v);

        m_loader.GenerateChunk(hit_chunk);
    }
}

void World::Place(const Vec3f &ray_start, const Vec3f &ray_direction, Voxel::Type type, const std::function<bool(const Vec3i&)> &should_place)
{
    Vec3ui previous, hit;
    Chunk *previous_chunk, *hit_chunk;

    if (DoRaycast(ray_start, ray_direction, &previous_chunk, &hit_chunk, previous, hit) && previous_chunk && should_place({ previous_chunk->GetX() + previous.x, static_cast<i32>(previous.y), previous_chunk->GetZ() + previous.z }))
    {
        // Place block and update chunk
        Voxel &v = previous_chunk->GetBlock(previous);
        v.type = type;

        m_db.AddBlock(previous_chunk->GetX(), previous_chunk->GetZ(), previous, v);

        m_loader.GenerateChunk(previous_chunk);
    }
}

void World::Update(const Vec3f &camera_pos)
{
    int32_t cx = static_cast<int32_t>(std::floor(camera_pos.x / CHUNK_WIDTH));
    int32_t cz = static_cast<int32_t>(std::floor(camera_pos.z / CHUNK_WIDTH));

    int32_t cx_pos = cx * CHUNK_WIDTH;
    int32_t cz_pos = cz * CHUNK_WIDTH;

    // Remove old chunks
    {
        auto chunks = m_loader.GetChunks();
        for (ChunkInfo *info : chunks)
        {
            if (std::abs(info->chunk->GetX() - cx_pos) > RENDER_DISTANCE * CHUNK_WIDTH
                || std::abs(info->chunk->GetZ() - cz_pos) > RENDER_DISTANCE * CHUNK_WIDTH)
            {
                m_loader.DeleteChunkSync(info);
            }
        }
    }

    // Add new chunks
    std::vector<std::pair<int32_t, int32_t>> potential_chunks;
    for (int32_t ix = cx - RENDER_DISTANCE; ix <= cx + RENDER_DISTANCE; ++ix)
    for (int32_t iz = cz - RENDER_DISTANCE; iz <= cz + RENDER_DISTANCE; ++iz)
    {
        potential_chunks.push_back({ ix, iz });
    }

    // Sort by distance from camera
    std::sort(potential_chunks.begin(), potential_chunks.end(),
        [=](const std::pair<int32_t, int32_t> &p1, const std::pair<int32_t, int32_t> &p2)
        {
            int32_t dx1 = p1.first - cx;
            int32_t dx2 = p2.first - cx;
            int32_t dz1 = p1.second - cz;
            int32_t dz2 = p2.second - cz;

            return dx1 * dx1 + dz1 * dz1 < dx2 * dx2 + dz2 * dz2;
        });

    for (const auto &pos : potential_chunks)
    {
        int32_t pos_x = pos.first * CHUNK_WIDTH;
        int32_t pos_z = pos.second * CHUNK_WIDTH;

        m_loader.AddChunk(pos_x, pos_z);
    }
}

void World::Render()
{
    auto chunks = m_loader.GetChunks();

    // Render solid
    glEnable(GL_CULL_FACE);
    for (ChunkInfo *info : chunks)
    {
        if (!info->should_generate && !info->chunk->Loaded()) info->chunk->Load();
        info->chunk->RenderSolid();
    }

    // Render transparent
    glDisable(GL_CULL_FACE);
    for (ChunkInfo *info : chunks) info->chunk->RenderTransparent();

    // Render transparent
    for (ChunkInfo *info : chunks) info->chunk->RenderWater();
}