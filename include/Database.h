#pragma once

#include"Vec.h"

#include<string>
#include<cstdint>
#include<functional>

#include<SQLiteCpp/SQLiteCpp.h>

#define BLOCKS_DB_FILE "save/worlds.db"

class Voxel;

class BlockDB
{
public:
    BlockDB(const std::string &name);
    
    void AddBlock(int32_t cx, int32_t cz, const Vec3ui16 &pos, const Voxel &voxel);
    void QueryBlocks(int32_t cx, int32_t cz, const std::function<void(const Vec3ui16&, const Voxel&)> &callback);

private:
    SQLite::Database m_db;
    SQLite::Statement m_add_statement, m_query_statement;
};