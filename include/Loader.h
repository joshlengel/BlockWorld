#pragma once

#include"Core.h"

#include<vector>
#include<mutex>
#include<condition_variable>
#include<thread>
#include<atomic>
#include<functional>

class Chunk;

class ChunkInfo
{
public:
    ChunkInfo(Chunk *chunk, bool should_create, bool should_generate, bool should_delete);

    Chunk *chunk;

    bool should_create;
    bool should_generate;
    bool should_delete;
};

class Loader
{
public:
    Loader(const std::function<void(Chunk*)> &on_create, const std::function<void(Chunk*)> &on_generate);
    ~Loader();

    void AddChunk(i32 x, i32 z);
    void GenerateChunk(Chunk *chunk);
    void DeleteChunkSync(ChunkInfo *info);
    ChunkInfo *GetCreatedChunk(i32 x, i32 z);
    ChunkInfo *GetLoadedChunk(i32 x, i32 z);

    class Iterator
    {
    public:
        Iterator(const std::vector<ChunkInfo*> &chunks, std::mutex &mut);

        std::vector<ChunkInfo*>::const_iterator begin();
        std::vector<ChunkInfo*>::const_iterator end();
    
    private:
        const std::vector<ChunkInfo*> &m_chunks;
        std::unique_lock<std::mutex> m_lock;
    };

    Iterator GetChunks();

private:
    std::vector<ChunkInfo*> m_chunks;
    std::mutex m_mutex;
    std::condition_variable m_cv;
    std::atomic_bool m_running;

    std::function<void(Chunk*)> m_on_create;
    std::function<void(Chunk*)> m_on_generate;

    std::thread m_worker;
};