#pragma once

#include"Vec.h"

#include<string>

class Mat3;
class Mat4;

struct ShaderData;

class Shader
{
public:
    Shader(const std::string &vertex_file, const std::string &fragment_file);
    ~Shader();

    void DeclareUniform(const std::string &msg);

    void Bind();

    void SetUniform(const std::string &msg, int i);
    void SetUniform(const std::string &msg, float f);
    void SetUniform(const std::string &msg, const Vec2f &v);
    void SetUniform(const std::string &msg, const Vec3f &v);
    void SetUniform(const std::string &msg, const Vec4f &v);
    void SetUniform(const std::string &msg, const Mat3 &m);
    void SetUniform(const std::string &msg, const Mat4 &m);

private:
    ShaderData *m_data;
};