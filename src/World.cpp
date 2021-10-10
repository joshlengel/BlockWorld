#include"World.h"
#include"Chunk.h"
#include"Vec.h"

#include<thread>
#include<cmath>
#include<algorithm>

World::World(int32_t x, int32_t z, uint_fast32_t seed):
    m_running(true),
    m_noise(seed),
    m_generator(m_noise)
{
    for (int32_t ix = -RENDER_DISTANCE; ix <= RENDER_DISTANCE; ++ix)
    for (int32_t iz = -RENDER_DISTANCE; iz <= RENDER_DISTANCE; ++iz)
    {
        m_load_queue.insert(m_load_queue.begin(), { ix * CHUNK_WIDTH, iz * CHUNK_WIDTH });
    }

    std::thread loader_thread([this]()
    {
        while (m_running)
        {
            std::unique_lock<std::mutex> l1(m_loader_mutex);
            if (m_load_queue.empty()) m_loader_cv.wait(l1);
            if (!m_running) return;

            auto chunk_pos = m_load_queue.back();
            m_load_queue.pop_back();

            l1.unlock();

            // Load chunk
            Chunk *chunk = new Chunk(chunk_pos.first, chunk_pos.second);
            m_generator.Generate(*chunk);
            chunk->GenerateMesh();

            std::unique_lock<std::mutex> l2(m_loader_mutex);
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

            m_chunks.push_back(chunk);
        }
    });

    loader_thread.detach();
}

World::~World()
{
    m_running = false;
    std::unique_lock<std::mutex> l1(m_loader_mutex);
    m_load_queue.clear();
    m_loader_cv.notify_one();
    l1.unlock();

    std::unique_lock<std::mutex> l2(m_loader_mutex);
    for (Chunk *chunk : m_chunks) delete chunk;
}

void World::Update(const Vec3 &camera_pos)
{
    int32_t cx = static_cast<int32_t>(std::floor(camera_pos.x / CHUNK_WIDTH));
    int32_t cz = static_cast<int32_t>(std::floor(camera_pos.z / CHUNK_WIDTH));

    int32_t cx_pos = cx * CHUNK_WIDTH;
    int32_t cz_pos = cz * CHUNK_WIDTH;

    // Remove old chunks
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

    // Add new chunks
    for (int32_t ix = cx - RENDER_DISTANCE; ix <= cx + RENDER_DISTANCE; ++ix)
    for (int32_t iz = cz - RENDER_DISTANCE; iz <= cz + RENDER_DISTANCE; ++iz)
    {
        int32_t pos_x = ix * CHUNK_WIDTH;
        int32_t pos_z = iz * CHUNK_WIDTH;

        std::unique_lock<std::mutex> lock(m_loader_mutex);
        if (std::find_if(m_chunks.begin(), m_chunks.end(), [=](Chunk *c){ return c->GetX() == pos_x && c->GetZ() == pos_z; }) == m_chunks.end())
        {
            // Must add chunk
            m_load_queue.push_back({ pos_x, pos_z });
            m_loader_cv.notify_one();
        }
    }
}

void World::Render()
{
    std::unique_lock<std::mutex> lock(m_loader_mutex);

    for (Chunk *chunk : m_chunks)
    {
        if (!chunk->Loaded()) chunk->Load();
        chunk->Render();
    }
}