#include"mesh/Planet.h"

#include<vector>
#include<cmath>

#include<vec3.hpp>

constexpr const static float GOLDEN_RATIO = (1.0f + std::sqrt(5.0f)) * 0.5f;

PlanetModel::PlanetModel(uint32_t precision):
    m_v_buff_id(),
    m_i_buff_id()
{
    std::vector<glm::vec3> vertices =
    {
        { -GOLDEN_RATIO, 0.0f, -1.0f },
        {  GOLDEN_RATIO, 0.0f, -1.0f },
        { -GOLDEN_RATIO, 0.0f,  1.0f },
        {  GOLDEN_RATIO, 0.0f,  1.0f },
        { -1.0f, -GOLDEN_RATIO, 0.0f, },
        { -1.0f,  GOLDEN_RATIO, 0.0f, },
        {  1.0f, -GOLDEN_RATIO, 0.0f, },
        {  1.0f,  GOLDEN_RATIO, 0.0f, },
        { 0.0f, -1.0f, -GOLDEN_RATIO },
        { 0.0f, -1.0f,  GOLDEN_RATIO },
        { 0.0f,  1.0f, -GOLDEN_RATIO },
        { 0.0f,  1.0f,  GOLDEN_RATIO }
    };

    std::vector<uint32_t> indices =
    {
        0, 2, 5,
        0, 4, 2,
        3, 1, 7,
        3, 6, 1,
        5, 11, 7,
        5, 7, 10,
        4, 6, 9,
        4, 8, 6,
        10, 8, 0,
        10, 1, 8,
        11, 9, 3,
        11, 2, 9,
        5, 2, 11,
        7, 11, 3,
        5, 10, 0,
        7, 1, 10,
        2, 4, 9,
        9, 6, 3,
        0, 8, 4,
        8, 1, 6
    };

    m_num_indices = indices.size();
    

    glGenBuffers(1, &m_v_buff_id);
    glGenBuffers(1, &m_i_buff_id);

    glBindBuffer(GL_ARRAY_BUFFER, m_v_buff_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_i_buff_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

PlanetModel::~PlanetModel()
{
    glDeleteBuffers(1, &m_v_buff_id);
    glDeleteBuffers(1, &m_i_buff_id);
}

void PlanetModel::Render() const
{
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, nullptr);
}