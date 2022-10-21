#pragma once

#include <vector>

#include <glm/glm.hpp>

#include "Texture.h"

namespace BIEngine
{
    struct Vertex 
    {
        Vertex() {}
        Vertex(
            const glm::vec3& p,
            const glm::vec3& n,
            const glm::vec2& uv) :
            Position(p),
            Normal(n),
            TexCoords(uv) {}
        Vertex(
            float px, float py, float pz,
            float nx, float ny, float nz,
            float u, float v) :
            Position(px, py, pz),
            Normal(nx, ny, nz),
            TexCoords(u, v) {}


        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    class Mesh
    {
        friend class Renderer;
        friend class MeshGeometryGenerator;

    public:
        Mesh(const std::vector<Vertex>& vertices, const std::vector<unsigned int>& indices);

    private:
        void SetupMesh();

    private:
        std::vector<Vertex> m_vertices;
        std::vector<unsigned int> m_indices;

        unsigned int m_VAO, m_VBO, m_EBO;
    };

}