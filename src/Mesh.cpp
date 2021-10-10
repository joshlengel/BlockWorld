#include"Mesh.h"

#include<array>

#include<glad/glad.h>

#define EPSILON 0.001f

struct MeshData
{
    GLuint vao_id;
    GLuint buff_ids[2];
};

Mesh::Mesh():
    m_data(new MeshData),
    m_last_index(0)
{
    glGenVertexArrays(1, &m_data->vao_id);
    glGenBuffers(2, m_data->buff_ids);
}

Mesh::~Mesh()
{
    glDeleteVertexArrays(1, &m_data->vao_id);
    glDeleteBuffers(2, m_data->buff_ids);
}

void Mesh::AddFace(Face face, uint16_t tex_index, size_t x, size_t y, size_t z)
{
    std::array<float, 12> vertices;

    switch (face)
    {
        case Face::LEFT:
            vertices =
            {
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 1.0f,
                0.0f, 1.0f, 0.0f
            };
            break;
        
        case Face::RIGHT:
            vertices =
            {
                 1.0f, 0.0f, 0.0f,
                 1.0f, 0.0f, 1.0f,
                 1.0f, 1.0f, 0.0f,
                 1.0f, 1.0f, 1.0f
            };
            break;
        
        case Face::BOTTOM:
            vertices =
            {
                0.0f, 0.0f, 1.0f,
                1.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f
            };
            break;
        
        case Face::TOP:
            vertices =
            {
                0.0f,  1.0f, 0.0f,
                1.0f,  1.0f, 0.0f,
                0.0f,  1.0f, 1.0f,
                1.0f,  1.0f, 1.0f
            };
            break;
        
        case Face::BACK:
            vertices =
            {
                0.0f, 0.0f, 0.0f,
                1.0f, 0.0f, 0.0f,
                0.0f, 1.0f, 0.0f,
                1.0f, 1.0f, 0.0f
            };
            break;
        
        case Face::FRONT:
            vertices =
            {
                1.0f, 0.0f,  1.0f,
                0.0f, 0.0f,  1.0f,
                1.0f, 1.0f,  1.0f,
                0.0f, 1.0f,  1.0f
            };
            break;
    }

    float u = static_cast<float>(tex_index % TEXTURE_ATLAS_SIZE) / TEXTURE_ATLAS_SIZE + EPSILON;
    float v = static_cast<float>(static_cast<uint16_t>(tex_index / TEXTURE_ATLAS_SIZE)) / TEXTURE_ATLAS_SIZE + EPSILON;

    float duv = 1.0f / TEXTURE_ATLAS_SIZE - 2 * EPSILON;

    m_vertices.push_back(vertices[0] + x);
    m_vertices.push_back(vertices[1] + y);
    m_vertices.push_back(vertices[2] + z);

    m_vertices.push_back(u);
    m_vertices.push_back(v + duv);

    m_vertices.push_back(vertices[3] + x);
    m_vertices.push_back(vertices[4] + y);
    m_vertices.push_back(vertices[5] + z);

    m_vertices.push_back(u + duv);
    m_vertices.push_back(v + duv);

    m_vertices.push_back(vertices[6] + x);
    m_vertices.push_back(vertices[7] + y);
    m_vertices.push_back(vertices[8] + z);

    m_vertices.push_back(u);
    m_vertices.push_back(v);

    m_vertices.push_back(vertices[9] + x);
    m_vertices.push_back(vertices[10] + y);
    m_vertices.push_back(vertices[11] + z);

    m_vertices.push_back(u + duv);
    m_vertices.push_back(v);

    m_indices.push_back(m_last_index + 0);
    m_indices.push_back(m_last_index + 1);
    m_indices.push_back(m_last_index + 3);
    m_indices.push_back(m_last_index + 0);
    m_indices.push_back(m_last_index + 3);
    m_indices.push_back(m_last_index + 2);
    m_last_index += 4;
}

void Mesh::Generate()
{
    glBindVertexArray(m_data->vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_data->buff_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, (3 + 2) * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (3 + 2) * sizeof(float), reinterpret_cast<void*>(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->buff_ids[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    m_index_count = m_indices.size();

    m_vertices.clear();
    m_indices.clear();
    m_last_index = 0;
}

void Mesh::Render() const
{
    glBindVertexArray(m_data->vao_id);
    glDrawElements(GL_TRIANGLES, m_index_count, GL_UNSIGNED_INT, nullptr);
}