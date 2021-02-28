#include"mesh/Mesh.h"

#include<algorithm>
#include<cmath>

#include<glm.hpp>

constexpr const static float GOLDEN_RATIO = (1.0f + std::sqrt(5.0f)) * 0.5f;

IcoSphere::IcoSphere(uint32_t precision):
    m_vertices(
    {
        { glm::normalize(glm::vec3({ -GOLDEN_RATIO, 0.0f, -1.0f })) },
        { glm::normalize(glm::vec3({  GOLDEN_RATIO, 0.0f, -1.0f })) },
        { glm::normalize(glm::vec3({ -GOLDEN_RATIO, 0.0f,  1.0f })) },
        { glm::normalize(glm::vec3({  GOLDEN_RATIO, 0.0f,  1.0f })) },
        { glm::normalize(glm::vec3({ -1.0f, -GOLDEN_RATIO, 0.0f })) },
        { glm::normalize(glm::vec3({ -1.0f,  GOLDEN_RATIO, 0.0f })) },
        { glm::normalize(glm::vec3({  1.0f, -GOLDEN_RATIO, 0.0f })) },
        { glm::normalize(glm::vec3({  1.0f,  GOLDEN_RATIO, 0.0f })) },
        { glm::normalize(glm::vec3({ 0.0f, -1.0f, -GOLDEN_RATIO })) },
        { glm::normalize(glm::vec3({ 0.0f, -1.0f,  GOLDEN_RATIO })) },
        { glm::normalize(glm::vec3({ 0.0f,  1.0f, -GOLDEN_RATIO })) },
        { glm::normalize(glm::vec3({ 0.0f,  1.0f,  GOLDEN_RATIO })) }
    }),
    m_indices(
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
    })
{
    for (uint32_t i = 0; i < precision; ++i)
    {
        struct Edge
        {
            glm::vec3 v1, v2;
            uint32_t mid_index;
        };

        // Returns equals predicate for given edge
        const auto GenEdgeEquals = [](const Edge &e){ return [=](const Edge &edge){ return (edge.v1 == e.v1 && edge.v2 == e.v2) || (edge.v1 == e.v2 && edge.v2 == e.v1); }; };

        std::vector<Edge> edges;

        std::vector<uint32_t> new_indices;
        new_indices.reserve(m_indices.size() * 4);

        auto index_itr = m_indices.begin();

        while (index_itr != m_indices.end())
        {
            uint32_t i1 = *index_itr++;
            uint32_t i2 = *index_itr++;
            uint32_t i3 = *index_itr++;

            Vertex v1 = m_vertices[i1];
            Vertex v2 = m_vertices[i2];
            Vertex v3 = m_vertices[i3];

            Edge e1 = { v1.position, v2.position };
            Edge e2 = { v2.position, v3.position };
            Edge e3 = { v3.position, v1.position };

            auto edges_end = edges.end();
            auto prev_e1 = std::find_if(edges.begin(), edges.end(), GenEdgeEquals(e1));
            auto prev_e2 = std::find_if(edges.begin(), edges.end(), GenEdgeEquals(e2));
            auto prev_e3 = std::find_if(edges.begin(), edges.end(), GenEdgeEquals(e3));

            uint32_t vni1, vni2, vni3;
            Vertex vn1, vn2, vn3;

            if (prev_e1 == edges_end)
            {
                e1.mid_index = vni1 = m_vertices.size();
                vn1 = { glm::normalize((v1.position + v2.position) * 0.5f) };

                m_vertices.push_back(vn1);
                edges.push_back(e1);
            }
            else
            {
                vni1 = prev_e1->mid_index;
                vn1 = m_vertices[vni1];
            }

            if (prev_e2 == edges_end)
            {
                e2.mid_index = vni2 = m_vertices.size();
                vn2 = { glm::normalize((v2.position + v3.position) * 0.5f) };

                m_vertices.push_back(vn2);
                edges.push_back(e2);
            }
            else
            {
                vni2 = prev_e2->mid_index;
                vn2 = m_vertices[vni2];
            }

            if (prev_e3 == edges_end)
            {
                e3.mid_index = vni3 = m_vertices.size();
                vn3 = { glm::normalize((v3.position + v1.position) * 0.5f) };

                m_vertices.push_back(vn3);
                edges.push_back(e3);
            }
            else
            {
                vni3 = prev_e3->mid_index;
                vn3 = m_vertices[vni3];
            }

            new_indices.push_back(vni1);
            new_indices.push_back(vni2);
            new_indices.push_back(vni3);

            new_indices.push_back(i1);
            new_indices.push_back(vni1);
            new_indices.push_back(vni3);

            new_indices.push_back(i2);
            new_indices.push_back(vni2);
            new_indices.push_back(vni1);

            new_indices.push_back(i3);
            new_indices.push_back(vni3);
            new_indices.push_back(vni2);
        }

        m_indices = new_indices;
    }

    for (Vertex &v : m_vertices)
        v.normal = v.position;
}

std::vector<IcoSphere::Vertex> &IcoSphere::GetVertices() { return m_vertices; }
std::vector<uint32_t> &IcoSphere::GetIndices() { return m_indices; }