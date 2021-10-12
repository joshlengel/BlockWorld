#pragma once

#include"Core.h"
#include"Vec.h"

#include<string>
#include<functional>

#include<SQLiteCpp/SQLiteCpp.h>

#define BLOCKS_DB_FILE "save/worlds.db"

class Voxel;

class BlockDB
{
public:
    BlockDB(const std::string &name);
    
    void AddBlock(i32 cx, i32 cz, const Vec3ui &pos, const Voxel &voxel);
    void QueryBlocks(i32 cx, i32 cz, const std::function<void(const Vec3ui&, const Voxel&)> &callback);

private:
    SQLite::Database m_db;
    SQLite::Statement m_add_statement, m_query_statement;
};