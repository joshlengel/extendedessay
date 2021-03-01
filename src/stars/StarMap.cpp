#include"stars/StarMap.h"

#include<vector>
#include<cstdint>

#include<vec3.hpp>
#include<vec2.hpp>
#include<random>
#include<gtc/random.hpp>
#include<gtx/perpendicular.hpp>

StarMap::StarMap(uint32_t density, float dist, float max_width, float variance):
    m_num_indices(density * 4),
    m_v_buff_id(),
    m_i_buff_id()
{
    glGenBuffers(1, &m_v_buff_id);
    glGenBuffers(1, &m_i_buff_id);

    struct Vertex
    {
        glm::vec3 position;
        glm::vec2 texture_coords;
    };

    std::vector<Vertex> vertices;
    std::vector<uint32_t> indices;

    vertices.reserve(density * 4);
    indices.reserve(density * 6);

    std::random_device rd;
    std::normal_distribution<double> rdist(max_width, std::sqrt(variance));

    for (uint32_t i = 0; i < density; ++i)
    {
        glm::vec3 p = glm::sphericalRand(dist);
        float size = rdist(rd);

        glm::vec3 perp1 = glm::normalize(glm::perp(glm::sphericalRand(1.0f), p)) * size;
        glm::vec3 perp2 = glm::normalize(glm::cross(perp1, p)) * size;
        glm::vec3 c1 = p + perp1;
        glm::vec3 c2 = p - perp1;
        glm::vec3 c3 = p + perp2;
        glm::vec3 c4 = p - perp2;

        uint32_t index = vertices.size();

        vertices.push_back({ c1, glm::vec2(0.0f, 1.0f) });
        vertices.push_back({ c2, glm::vec2(1.0f, 0.0f) });
        vertices.push_back({ c3, glm::vec2(1.0f, 1.0f) });
        vertices.push_back({ c4, glm::vec2(0.0f, 0.0f) });

        indices.push_back(index);
        indices.push_back(index + 3);
        indices.push_back(index + 1);
        indices.push_back(index);
        indices.push_back(index + 1);
        indices.push_back(index + 2);
    }

    glBindBuffer(GL_ARRAY_BUFFER, m_v_buff_id);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, position)));
    glVertexAttribPointer(1, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texture_coords)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_i_buff_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), indices.data(), GL_STATIC_DRAW);
}

StarMap::~StarMap()
{
    glDeleteBuffers(1, &m_v_buff_id);
    glDeleteBuffers(1, &m_i_buff_id);
}

void StarMap::Render() const
{
    Model::Bind();
    glDrawElements(GL_TRIANGLES, m_num_indices, GL_UNSIGNED_INT, nullptr);
}