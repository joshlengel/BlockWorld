#include"Quad.h"

#include<glad/glad.h>

struct QuadRendererData
{
    GLuint vao_id;
    GLuint buff_ids[2];
};

QuadRenderer::QuadRenderer():
    m_data(new QuadRendererData),
    m_vertices(),
    m_indices(),
    m_last_index(0)
{
    glGenVertexArrays(1, &m_data->vao_id);
    glGenBuffers(2, m_data->buff_ids);
}

QuadRenderer::~QuadRenderer()
{
    glDeleteVertexArrays(1, &m_data->vao_id);
    glDeleteBuffers(2, m_data->buff_ids);

    delete m_data;
}

void QuadRenderer::Add(const Quad &quad)
{
    float hw = quad.width * 0.5f;
    float hh = quad.height * 0.5f;

    m_vertices.push_back(quad.x - hw);
    m_vertices.push_back(quad.y - hh);
    m_vertices.push_back(0.0f);
    m_vertices.push_back(1.0f);

    m_vertices.push_back(quad.r);
    m_vertices.push_back(quad.g);
    m_vertices.push_back(quad.b);
    m_vertices.push_back(quad.a);

    m_vertices.push_back(quad.x + hw);
    m_vertices.push_back(quad.y - hh);
    m_vertices.push_back(1.0f);
    m_vertices.push_back(1.0f);

    m_vertices.push_back(quad.r);
    m_vertices.push_back(quad.g);
    m_vertices.push_back(quad.b);
    m_vertices.push_back(quad.a);

    m_vertices.push_back(quad.x - hw);
    m_vertices.push_back(quad.y + hh);
    m_vertices.push_back(0.0f);
    m_vertices.push_back(0.0f);

    m_vertices.push_back(quad.r);
    m_vertices.push_back(quad.g);
    m_vertices.push_back(quad.b);
    m_vertices.push_back(quad.a);

    m_vertices.push_back(quad.x + hw);
    m_vertices.push_back(quad.y + hh);
    m_vertices.push_back(1.0f);
    m_vertices.push_back(0.0f);

    m_vertices.push_back(quad.r);
    m_vertices.push_back(quad.g);
    m_vertices.push_back(quad.b);
    m_vertices.push_back(quad.a);

    m_indices.push_back(m_last_index + 0);
    m_indices.push_back(m_last_index + 1);
    m_indices.push_back(m_last_index + 3);
    m_indices.push_back(m_last_index + 0);
    m_indices.push_back(m_last_index + 3);
    m_indices.push_back(m_last_index + 2);
    m_last_index += 4;
}

void QuadRenderer::Render()
{
    glBindVertexArray(m_data->vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_data->buff_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(float), m_vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2+2+4) * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (2+2+4) * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, (2+2+4) * sizeof(float), reinterpret_cast<void*>(4 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->buff_ids[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), m_indices.data(), GL_STATIC_DRAW);

    glDrawElements(GL_TRIANGLES, m_last_index, GL_UNSIGNED_INT, nullptr);
}