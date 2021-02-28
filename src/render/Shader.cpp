#include"render/Shader.h"

#include<fstream>
#include<iostream>
#include<sstream>

static const GLenum SHADER_TYPES[] =
{
    GL_VERTEX_SHADER,
    GL_GEOMETRY_SHADER,
    GL_FRAGMENT_SHADER
};

Shader::Shader(ShaderType type, const std::string &source):
    m_type(type),
    m_id()
{
    m_id = glCreateShader(SHADER_TYPES[static_cast<uint32_t>(type)]);

    const char *src = source.c_str();
    glShaderSource(m_id, 1, &src, nullptr);
    glCompileShader(m_id);

    GLint status;
    glGetShaderiv(m_id, GL_COMPILE_STATUS, &status);

    if (!status)
    {
        std::cerr << "Error compiling shader. Error message:\n";

        GLint length;
        glGetShaderiv(m_id, GL_INFO_LOG_LENGTH, &length);

        char *buff = new char[length];
        glGetShaderInfoLog(m_id, length, nullptr, buff);

        std::cerr << buff << std::endl;
        delete[] buff;
        return;
    }
}

Shader::~Shader()
{
    glDeleteShader(m_id);
}

Shader *Shader::Load(ShaderType type, const std::string &path)
{
    std::ifstream file(path);

    if (!file)
    {
        std::cerr << "Error opening file '" << path << "'" << std::endl;
    }

    std::stringstream source;
    std::string line;

    while (std::getline(file, line))
    {
        source << line << '\n';
    }

    return new Shader(type, source.str());
}

ShaderProgram::ShaderProgram():
    m_id(),
    m_shaders(),
    m_uniform_locations()
{
    m_id = glCreateProgram();
}

ShaderProgram::~ShaderProgram()
{
    for (Shader *shader : m_shaders)
    {
        glDetachShader(m_id, shader->m_id);
        delete shader;
    }

    glDeleteProgram(m_id);
}

void ShaderProgram::AttachShader(Shader *shader)
{
    m_shaders.push_back(shader);

    glAttachShader(m_id, shader->m_id);
}

void ShaderProgram::DeclareUniform(const std::string &name)
{
    m_uniform_locations[name] = glGetUniformLocation(m_id, name.c_str());
}

void ShaderProgram::DeclareUniform(std::string &&name)
{
    m_uniform_locations[std::forward<std::string>(name)] = glGetUniformLocation(m_id, name.c_str());
}

void ShaderProgram::Make()
{
    glLinkProgram(m_id);

    GLint status;
    glGetProgramiv(m_id, GL_LINK_STATUS, &status);

    if (!status)
    {
        std::cerr << "Error linking shader program. Error message:\n";

        GLint length;
        glGetProgramiv(m_id, GL_INFO_LOG_LENGTH, &length);

        char *buff = new char[length];
        glGetProgramInfoLog(m_id, length, nullptr, buff);

        std::cerr << buff << std::endl;
        delete[] buff;
        return;
    }
}

void ShaderProgram::Bind()
{
    glUseProgram(m_id);
}

void ShaderProgram::SetUniform(const std::string &name, float f) { glUniform1f(m_uniform_locations[name], f); }
void ShaderProgram::SetUniform(const std::string &name, glm::vec2 v) { glUniform2f(m_uniform_locations[name], v.x, v.y); }
void ShaderProgram::SetUniform(const std::string &name, glm::vec3 v) { glUniform3f(m_uniform_locations[name], v.x, v.y, v.z); }
void ShaderProgram::SetUniform(const std::string &name, glm::vec4 v) { glUniform4f(m_uniform_locations[name], v.x, v.y, v.z, v.w); }
void ShaderProgram::SetUniform(const std::string &name, glm::mat3 m) { glUniformMatrix3fv(m_uniform_locations[name], 1, false, &m[0][0]); }
void ShaderProgram::SetUniform(const std::string &name, glm::mat4 m) { glUniformMatrix4fv(m_uniform_locations[name], 1, false, &m[0][0]); }