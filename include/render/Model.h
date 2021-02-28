#pragma once

#include<glad/glad.h>

class Renderable
{
public:
    virtual ~Renderable() {}

    virtual void Render() const = 0;
};

class Model : public virtual Renderable
{
public:
    Model();
    virtual ~Model();

    virtual void Bind() const;
    virtual void Render() const = 0;

private:
    GLuint m_vao_id;
};