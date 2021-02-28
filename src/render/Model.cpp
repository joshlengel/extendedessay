#include"render/Model.h"

Model::Model():
    m_vao_id()
{
    glGenVertexArrays(1, &m_vao_id);
    Bind();
}

Model::~Model()
{
    glDeleteVertexArrays(1, &m_vao_id);
}

void Model::Bind() const
{
    glBindVertexArray(m_vao_id);
}