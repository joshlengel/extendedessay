#include"mesh/MeshModel.h"

IcoModel::IcoModel(uint32_t precision):
    m_mesh(precision),
    m_num_indices(m_mesh.GetIndices().size()),
    m_v_buff_id(),
    m_i_buff_id()
{
    glGenBuffers(1, &m_v_buff_id);
    glGenBuffers(1, &m_i_buff_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_v_buff_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(IcoSphere::Vertex) * m_mesh.GetVertices().size(), m_mesh.GetVertices().data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(IcoSphere::Vertex), reinterpret_cast<void*>(offsetof(IcoSphere::Vertex, position)));
    glVertexAttribPointer(1, 3, GL_FLOAT, false, sizeof(IcoSphere::Vertex), reinterpret_cast<void*>(offsetof(IcoSphere::Vertex, normal)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_i_buff_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * m_mesh.GetIndices().size(), m_mesh.GetIndices().data(), GL_STATIC_DRAW);
}

IcoModel::~IcoModel()
{
    glDeleteBuffers(1, &m_v_buff_id);
    glDeleteBuffers(1, &m_i_buff_id);
}

void IcoModel::Render() const
{
    Model::Bind();
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, nullptr);
};