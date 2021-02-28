#pragma once

#include<vector>
#include<cstdint>

#include<vec3.hpp>

class IcoSphere
{
public:
    IcoSphere(uint32_t precision);

    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
    };

    std::vector<Vertex> &GetVertices();
    std::vector<uint32_t> &GetIndices();

private:
    std::vector<Vertex> m_vertices;
    std::vector<uint32_t> m_indices;
};