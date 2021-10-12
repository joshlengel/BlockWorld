#include"Loader.h"
#include"Chunk.h"

#include<algorithm>

ChunkInfo::ChunkInfo(Chunk *chunk, bool should_create, bool should_generate, bool should_delete):
    chunk(chunk),
    should_create(should_create),
    should_generate(should_generate),
    should_delete(should_delete)
{}

Loader::Iterator::Iterator(const std::vector<ChunkInfo*> &chunks, std::mutex &mut):
    m_chunks(chunks),
    m_lock(mut)
{}

std::vector<ChunkInfo*>::const_iterator Loader::Iterator::begin() { return m_chunks.begin(); }
std::vector<ChunkInfo*>::const_iterator Loader::Iterator::end() { return m_chunks.end(); }

Loader::Loader(const std::function<void(Chunk*)> &on_create, const std::function<void(Chunk*)> &on_generate):
    m_chunks(),
    m_mutex(),
    m_cv(),
    m_running(true),
    m_on_create(on_create),
    m_on_generate(on_generate),
    m_worker([this]()
    {
        while (m_running)
        {
            std::unique_lock<std::mutex> lock(m_mutex);
            auto itr = std::find_if(m_chunks.begin(), m_chunks.end(), [](ChunkInfo *info){ return info->should_create || info->should_generate || info->should_delete; });
            if (itr == m_chunks.end())
            {
                m_cv.wait(lock);
                continue;
            }

            ChunkInfo *info = *itr;

            if (info->should_delete)
            {
                m_chunks.erase(itr);
                delete info->chunk;
                delete info;
                continue;
            }
            lock.unlock();

            if (info->should_create)
            {
                m_on_create(info->chunk);
                info->should_create = false;
            }

            if (info->should_generate)
            {
                m_on_generate(info->chunk);
                info->should_generate = false;
            }
        }
    })
{}

Loader::~Loader()
{
    m_running = false;
    std::unique_lock<std::mutex> lock(m_mutex);
    m_cv.notify_one();
    lock.unlock();

    m_worker.join();

    for (ChunkInfo *info : m_chunks)
    {
        delete info->chunk;
        delete info;
    }
}

void Loader::AddChunk(i32 x, i32 z)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    // Don't add duplicates
    if (std::find_if(m_chunks.begin(), m_chunks.end(), [=](ChunkInfo *info){ return info->chunk->GetX() == x && info->chunk->GetZ() == z; }) != m_chunks.end()) return;
    
    Chunk *chunk = new Chunk(x, z);

    m_chunks.push_back(new ChunkInfo(chunk, true, true, false));
    m_cv.notify_one();
}

void Loader::GenerateChunk(Chunk *chunk)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto itr = std::find_if(m_chunks.begin(), m_chunks.end(), [=](ChunkInfo *info){ return info->chunk == chunk; });

    if (itr != m_chunks.end())
    {
        ChunkInfo *info = *itr;
        info->should_generate = true;
        m_cv.notify_one();
    }
}

void Loader::DeleteChunkSync(ChunkInfo *info)
{
    info->should_delete = true;
    m_cv.notify_one();
}

ChunkInfo *Loader::GetCreatedChunk(i32 x, i32 z)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto itr = std::find_if(m_chunks.begin(), m_chunks.end(), [=](ChunkInfo *info)
    {
        return info->chunk->GetX() == x && info->chunk->GetZ() == z && !info->should_create && !info->should_delete;
    });

    return itr == m_chunks.end()? nullptr : *itr;
}

ChunkInfo *Loader::GetLoadedChunk(i32 x, i32 z)
{
    std::unique_lock<std::mutex> lock(m_mutex);
    auto itr = std::find_if(m_chunks.begin(), m_chunks.end(), [=](ChunkInfo *info)
    {
        return info->chunk->GetX() == x && info->chunk->GetZ() == z && !info->should_create && !info->should_generate && !info->should_delete;
    });

    return itr == m_chunks.end()? nullptr : *itr;
}

Loader::Iterator Loader::GetChunks() { return Iterator(m_chunks, m_mutex); }