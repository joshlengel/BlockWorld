#include"Shader.h"
#include"Vec.h"
#include"Mat.h"

#include<fstream>
#include<sstream>
#include<map>

#include<glad/glad.h>

struct ShaderData
{
    GLuint program_id;
    GLuint vertex_id;
    GLuint fragment_id;

    std::map<std::string, GLint> locations;
};

static std::string ReadFile(const std::string &file)
{
    std::ifstream in(file);
    if (!in) throw std::runtime_error("Error reading asset '" + file + "'");

    std::stringstream ss;
    std::string line;

    while (std::getline(in, line))
    {
        ss << line << '\n';
    }

    return ss.str();
}

Shader::Shader(const std::string &vertex_file, const std::string &fragment_file):
    m_data(new ShaderData)
{
    m_data->program_id = glCreateProgram();
    m_data->vertex_id = glCreateShader(GL_VERTEX_SHADER);
    m_data->fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

    std::string v_str = ReadFile(vertex_file);
    std::string f_str = ReadFile(fragment_file);
    const char *v_c_str = v_str.c_str();
    const char *f_c_str = f_str.c_str();

    glShaderSource(m_data->vertex_id, 1, &v_c_str, nullptr);
    glShaderSource(m_data->fragment_id, 1, &f_c_str, nullptr);

    glCompileShader(m_data->vertex_id);
    glCompileShader(m_data->fragment_id);

    GLint status;

    glGetShaderiv(m_data->vertex_id, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        GLint length;
        glGetShaderiv(m_data->vertex_id, GL_INFO_LOG_LENGTH, &length);
        std::string msg(length, '\0');
        glGetShaderInfoLog(m_data->vertex_id, length, nullptr, &msg[0]);

        throw std::runtime_error("Error compiling vertex shader '" + vertex_file + "': " + msg);
    }

    glGetShaderiv(m_data->fragment_id, GL_COMPILE_STATUS, &status);
    if (!status)
    {
        GLint length;
        glGetShaderiv(m_data->fragment_id, GL_INFO_LOG_LENGTH, &length);
        std::string msg(length, '\0');
        glGetShaderInfoLog(m_data->fragment_id, length, nullptr, &msg[0]);

        throw std::runtime_error("Error compiling fragment shader '" + fragment_file + "': " + msg);
    }

    glAttachShader(m_data->program_id, m_data->vertex_id);
    glAttachShader(m_data->program_id, m_data->fragment_id);

    glLinkProgram(m_data->program_id);

    glGetProgramiv(m_data->program_id, GL_LINK_STATUS, &status);
    if (!status)
    {
        GLint length;
        glGetProgramiv(m_data->program_id, GL_INFO_LOG_LENGTH, &length);
        std::string msg(length, '\0');
        glGetProgramInfoLog(m_data->program_id, length, nullptr, &msg[0]);

        throw std::runtime_error("Error linking shader program: " + msg);
    }

    glDetachShader(m_data->program_id, m_data->vertex_id);
    glDetachShader(m_data->program_id, m_data->fragment_id);
}

Shader::~Shader()
{
    glDeleteProgram(m_data->program_id);
    glDeleteShader(m_data->vertex_id);
    glDeleteShader(m_data->fragment_id);

    delete m_data;
}

void Shader::DeclareUniform(const std::string &msg)
{
    m_data->locations[msg] = glGetUniformLocation(m_data->program_id, msg.c_str());
}

void Shader::Bind()
{
    glUseProgram(m_data->program_id);
}

void Shader::SetUniform(const std::string &msg, float f)
{
    glUniform1f(m_data->locations[msg], f);
}

void Shader::SetUniform(const std::string &msg, const Vec2f &v)
{
    glUniform2f(m_data->locations[msg], v.x, v.y);
}

void Shader::SetUniform(const std::string &msg, const Vec3f &v)
{
    glUniform3f(m_data->locations[msg], v.x, v.y, v.z);
}

void Shader::SetUniform(const std::string &msg, const Vec4f &v)
{
    glUniform4f(m_data->locations[msg], v.x, v.y, v.z, v.w);
}

void Shader::SetUniform(const std::string &msg, const Mat3 &m)
{
    glUniformMatrix3fv(m_data->locations[msg], 1, GL_TRUE, &m.m00);
}

void Shader::SetUniform(const std::string &msg, const Mat4 &m)
{
    glUniformMatrix4fv(m_data->locations[msg], 1, GL_TRUE, &m.m00);
}