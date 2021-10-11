#include"Database.h"
#include"Chunk.h"

#include<cstdint>
#include<sstream>

static SQLite::Database &InitDB(SQLite::Database &db, const std::string &name)
{
    db.exec("CREATE TABLE IF NOT EXISTS " + name + " (cx INT, cz INT, x INT, y INT, z INT, type INT, UNIQUE(cx,cz,x,y,z))");
    return db;
}

BlockDB::BlockDB(const std::string &name):
    m_db(BLOCKS_DB_FILE, SQLite::OPEN_READWRITE | SQLite::OPEN_CREATE),
    m_add_statement(InitDB(m_db, name), "INSERT INTO " + name + " VALUES (?1, ?2, ?3, ?4, ?5, ?6) ON CONFLICT DO UPDATE SET type=?6"),
    m_query_statement(m_db, "SELECT * FROM " + name + " WHERE cx=? AND cz=?")
{}

void BlockDB::AddBlock(int32_t cx, int32_t cz, const Vec3ui16 &pos, const Voxel &voxel)
{
    m_add_statement.bind(1, cx);
    m_add_statement.bind(2, cz);
    m_add_statement.bind(3, pos.x);
    m_add_statement.bind(4, pos.y);
    m_add_statement.bind(5, pos.z);
    m_add_statement.bind(6, static_cast<uint16_t>(voxel.type));

    m_add_statement.exec();
    m_add_statement.reset();
}

void BlockDB::QueryBlocks(int32_t cx, int32_t cz, const std::function<void(const Vec3ui16&, const Voxel&)> &callback)
{
    m_query_statement.bind(1, cx);
    m_query_statement.bind(2, cz);

    while (m_query_statement.executeStep())
    {
        Voxel v{ static_cast<Voxel::Type>(static_cast<uint16_t>(m_query_statement.getColumn(5))) };
        callback({ m_query_statement.getColumn(2), m_query_statement.getColumn(3), m_query_statement.getColumn(4) }, v);
    }

    m_query_statement.reset();
}