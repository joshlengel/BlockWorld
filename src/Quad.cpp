#include"Quad.h"
#include"Vec.h"

#include<glad/glad.h>

static const uint8_t WHITE_PIXELS[] = { 255U, 255U, 255U, 255U };

struct QuadRendererData
{
    GLuint vao_id;
    GLuint buff_ids[2];
};

QuadRenderer::QuadRenderer():
    m_data(new QuadRendererData),
    m_shader("assets/quadShader.vert", "assets/quadShader.frag"),
    m_default_texture(Texture::FromPixels(1, 1, WHITE_PIXELS))
{
    glGenVertexArrays(1, &m_data->vao_id);
    glGenBuffers(2, m_data->buff_ids);

    float vertices[] =
    {
        -1.0f, -1.0f,   0.0f, 1.0f,
         1.0f, -1.0f,   1.0f, 1.0f,
        -1.0f,  1.0f,   0.0f, 0.0f,
         1.0f,  1.0f,   1.0f, 0.0f
    };

    unsigned int indices[] =
    {
        0, 1, 3, 0, 3, 2
    };

    glBindVertexArray(m_data->vao_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_data->buff_ids[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, (2+2) * sizeof(float), reinterpret_cast<void*>(0 * sizeof(float)));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, (2+2) * sizeof(float), reinterpret_cast<void*>(2 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_data->buff_ids[1]);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    m_shader.DeclareUniform("offset");
    m_shader.DeclareUniform("scale");
    m_shader.DeclareUniform("color");
}

QuadRenderer::~QuadRenderer()
{
    glDeleteVertexArrays(1, &m_data->vao_id);
    glDeleteBuffers(2, m_data->buff_ids);

    delete m_data;
}

void QuadRenderer::Add(const Quad &quad)
{
    m_quads.push_back(quad);
}

void QuadRenderer::Flush()
{
    m_quads.clear();
}

void QuadRenderer::Render()
{
    m_shader.Bind();

    glBindVertexArray(m_data->vao_id);

    for (const Quad &quad : m_quads)
    {
        Texture *texture = quad.texture? quad.texture : &m_default_texture;
        texture->Bind();

        m_shader.SetUniform("offset", { quad.x, quad.y });
        m_shader.SetUniform("scale", { quad.width, quad.height });
        m_shader.SetUniform("color", { quad.r, quad.g, quad.b, quad.a });

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
    }
}