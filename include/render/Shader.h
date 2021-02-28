#pragma once

#include<vector>
#include<unordered_map>
#include<string>

#include<cstdint>

#include<glad/glad.h>
#include<vec2.hpp>
#include<vec3.hpp>
#include<vec4.hpp>
#include<mat3x3.hpp>
#include<mat4x4.hpp>

enum class ShaderType : uint32_t
{
    VERTEX,
    GEOMETRY,
    FRAGMENT
};

class Shader
{
    friend class ShaderProgram;
public:
    Shader(ShaderType type, const std::string &source);
    ~Shader();

    static Shader *Load(ShaderType type, const std::string &path);

private:
    ShaderType m_type;
    GLuint m_id;
};

class ShaderProgram
{
public:
    ShaderProgram();
    ~ShaderProgram();

    void AttachShader(Shader *shader);
    void DeclareUniform(const std::string &name);
    void DeclareUniform(std::string &&name);
    void Make();

    void Bind();
    void SetUniform(const std::string &name, float f);
    void SetUniform(const std::string &name, glm::vec2 v);
    void SetUniform(const std::string &name, glm::vec3 v);
    void SetUniform(const std::string &name, glm::vec4 v);
    void SetUniform(const std::string &name, glm::mat3 m);
    void SetUniform(const std::string &name, glm::mat4 m);

private:
    GLuint m_id;
    std::vector<Shader*> m_shaders;
    std::unordered_map<std::string, GLint> m_uniform_locations;
};