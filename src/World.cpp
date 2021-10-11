#include"World.h"
#include"Chunk.h"
#include"Vec.h"
#include"Database.h"

#include<thread>
#include<cmath>
#include<algorithm>
#include<iostream>

World::World(int32_t x, int32_t z, BlockDB &db, uint_fast32_t seed):
    m_running(true),
    m_noise(seed),
    m_generator(m_noise, db),
    m_db(db)
{
    for (int32_t ix = -RENDER_DISTANCE; ix <= RENDER_DISTANCE; ++ix)
    for (int32_t iz = -RENDER_DISTANCE; iz <= RENDER_DISTANCE; ++iz)
    {
        m_create_queue.insert(m_create_queue.begin(), { ix * CHUNK_WIDTH, iz * CHUNK_WIDTH });
    }

    std::thread create_thread([this]()
    {
        while (m_running)
        {
            std::unique_lock<std::mutex> l1(m_create_mutex);
            if (m_create_queue.empty()) m_create_cv.wait(l1);
            if (!m_running) return;

            auto chunk_pos = m_create_queue.back();
            m_create_queue.pop_back();

            l1.unlock();

            // Load chunk
            Chunk *chunk = new Chunk(chunk_pos.first, chunk_pos.second);
            m_generator.Generate(*chunk);

            std::unique_lock<std::mutex> l2(m_chunk_mutex);
            m_chunks.push_back(chunk);
            l2.unlock();

            std::unique_lock<std::mutex> l3(m_generate_mutex);
            m_generate_queue.insert(m_generate_queue.begin(), chunk);
            m_generate_cv.notify_one();
        }
    });

    std::thread generate_thread([this]()
    {
        while (m_running)
        {
            std::unique_lock<std::mutex> l1(m_generate_mutex);
            if (m_generate_queue.empty()) m_generate_cv.wait(l1);
            if (!m_running) return;

            Chunk *chunk = m_generate_queue.back();
            m_generate_queue.pop_back();

            l1.unlock();

            // Load chunk
            chunk->GenerateMesh();

            std::unique_lock<std::mutex> l2(m_chunk_mutex);
            // Find loaded neighbors
            for (Chunk *neighbor : m_chunks)
            {
                if (neighbor->GetX() == chunk->GetX() - CHUNK_WIDTH && neighbor->GetZ() == chunk->GetZ())
                {
                    chunk->GenerateBorder(Face::LEFT, neighbor);
                    neighbor->GenerateBorder(Face::RIGHT, chunk);
                }
                else if (neighbor->GetX() == chunk->GetX() + CHUNK_WIDTH && neighbor->GetZ() == chunk->GetZ())
                {
                    chunk->GenerateBorder(Face::RIGHT, neighbor);
                    neighbor->GenerateBorder(Face::LEFT, chunk);
                }
                else if (neighbor->GetX() == chunk->GetX() && neighbor->GetZ() == chunk->GetZ() - CHUNK_WIDTH)
                {
                    chunk->GenerateBorder(Face::BACK, neighbor);
                    neighbor->GenerateBorder(Face::FRONT, chunk);
                }
                else if (neighbor->GetX() == chunk->GetX() && neighbor->GetZ() == chunk->GetZ() + CHUNK_WIDTH)
                {
                    chunk->GenerateBorder(Face::FRONT, neighbor);
                    neighbor->GenerateBorder(Face::BACK, chunk);
                }
            }
        }
    });

    create_thread.detach();
    generate_thread.detach();
}

World::~World()
{
    m_running = false;
    std::unique_lock<std::mutex> l1(m_create_mutex);
    m_create_queue.clear();
    m_create_cv.notify_one();
    l1.unlock();

    std::unique_lock<std::mutex> l2(m_generate_mutex);
    m_generate_queue.clear();
    m_generate_cv.notify_one();
    l2.unlock();

    std::unique_lock<std::mutex> l3(m_chunk_mutex);
    for (Chunk *chunk : m_chunks) delete chunk;
}

Voxel *World::GetBlock(const Vec3f &position)
{
    if (position.y < 0 || position.y >= CHUNK_HEIGHT) return nullptr;

    int32_t cx = static_cast<int32_t>(std::floor(position.x / CHUNK_WIDTH)) * CHUNK_WIDTH;
    int32_t cz = static_cast<int32_t>(std::floor(position.z / CHUNK_WIDTH)) * CHUNK_WIDTH;

    std::unique_lock<std::mutex> lock(m_chunk_mutex);
    auto itr = std::find_if(m_chunks.begin(), m_chunks.end(), [=](Chunk *chunk){ return cx == chunk->GetX() && cz == chunk->GetZ(); });
    if (itr != m_chunks.end())
    {
        Vec3ui16 pos =
        {
            static_cast<uint16_t>(position.x - cx),
            static_cast<uint16_t>(position.y),
            static_cast<uint16_t>(position.z - cz)
        };

        return &(*itr)->GetBlock(pos);
    }

    return nullptr;
}

bool World::DoRaycast(const Vec3f &ray_start, const Vec3f &ray_direction, Chunk **previous_chunk, Chunk **hit_chunk, Vec3ui16 &previous, Vec3ui16 &hit)
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

        std::unique_lock<std::mutex> lock(m_chunk_mutex);
        auto itr = std::find_if(m_chunks.begin(), m_chunks.end(), [=](Chunk *chunk){ return cx == chunk->GetX() && cz == chunk->GetZ(); });
        if (itr != m_chunks.end())
        {
            Vec3ui16 pos =
            {
                static_cast<uint16_t>(position.x - cx),
                static_cast<uint16_t>(position.y),
                static_cast<uint16_t>(position.z - cz)
            };

            Voxel &voxel = (*itr)->GetBlock(pos);

            previous = hit;
            *previous_chunk = *hit_chunk;

            hit = pos;
            *hit_chunk = *itr;

            if (voxel.type != Voxel::Type::AIR) return true;
        }
    }

    *previous_chunk = *hit_chunk = nullptr;
    return false;
}

void World::Break(const Vec3f &ray_start, const Vec3f &ray_direction)
{
    Vec3ui16 previous, hit;
    Chunk *previous_chunk, *hit_chunk;
    
    if (DoRaycast(ray_start, ray_direction, &previous_chunk, &hit_chunk, previous, hit))
    {
        // Break block and update chunk
        Voxel &v = hit_chunk->GetBlock(hit);
        v.type = Voxel::Type::AIR;
        
        m_db.AddBlock(hit_chunk->GetX(), hit_chunk->GetZ(), hit, v);

        std::unique_lock<std::mutex> lock(m_generate_mutex);
        m_generate_queue.insert(m_generate_queue.begin(), hit_chunk);
        m_generate_cv.notify_one();
    }
}

void World::Place(const Vec3f &ray_start, const Vec3f &ray_direction, Voxel::Type type, const std::function<bool(const Vec3f&)> &should_place)
{
    Vec3ui16 previous, hit;
    Chunk *previous_chunk, *hit_chunk;

    if (DoRaycast(ray_start, ray_direction, &previous_chunk, &hit_chunk, previous, hit) && previous_chunk && should_place({ previous_chunk->GetX() + previous.x, previous.y, previous_chunk->GetZ() + previous.z }))
    {
        // Place block and update chunk
        Voxel &v = previous_chunk->GetBlock(previous);
        v.type = type;

        m_db.AddBlock(previous_chunk->GetX(), previous_chunk->GetZ(), previous, v);

        std::unique_lock<std::mutex> lock(m_generate_mutex);
        m_generate_queue.insert(m_generate_queue.begin(), previous_chunk);
        m_generate_cv.notify_one();
    }
}

void World::Update(const Vec3f &camera_pos)
{
    int32_t cx = static_cast<int32_t>(std::floor(camera_pos.x / CHUNK_WIDTH));
    int32_t cz = static_cast<int32_t>(std::floor(camera_pos.z / CHUNK_WIDTH));

    int32_t cx_pos = cx * CHUNK_WIDTH;
    int32_t cz_pos = cz * CHUNK_WIDTH;

    // Remove old chunks
    std::unique_lock<std::mutex> lock(m_chunk_mutex);
    size_t index = 0;

    while (index < m_chunks.size())
    {
        Chunk *chunk = m_chunks[index];

        if (std::abs(chunk->GetX() - cx_pos) > RENDER_DISTANCE * CHUNK_WIDTH || std::abs(chunk->GetZ() - cz_pos) > RENDER_DISTANCE * CHUNK_WIDTH)
        {
            delete chunk;
            m_chunks.erase(m_chunks.begin() + index);
        }
        else
        {
            ++index;
        }
    }
    lock.unlock();

    // Add new chunks
    for (int32_t ix = cx - RENDER_DISTANCE; ix <= cx + RENDER_DISTANCE; ++ix)
    for (int32_t iz = cz - RENDER_DISTANCE; iz <= cz + RENDER_DISTANCE; ++iz)
    {
        int32_t pos_x = ix * CHUNK_WIDTH;
        int32_t pos_z = iz * CHUNK_WIDTH;

        std::unique_lock<std::mutex> lock(m_create_mutex);
        if (std::find_if(m_chunks.begin(), m_chunks.end(), [=](Chunk *c){ return c->GetX() == pos_x && c->GetZ() == pos_z; }) == m_chunks.end())
        {
            // Must add chunk
            m_create_queue.push_back({ pos_x, pos_z });
            m_create_cv.notify_one();
        }
    }
}

void World::Render()
{
    std::unique_lock<std::mutex> lock(m_chunk_mutex);

    for (Chunk *chunk : m_chunks)
    {
        if (!chunk->Loaded()) chunk->Load();
        chunk->Render();
    }
}