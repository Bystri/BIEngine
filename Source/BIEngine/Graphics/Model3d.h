#pragma once

#include <memory>

#include "Mesh.h"
#include "Texture.h"
#include "MeshGeometryGenerator.h"

namespace BIEngine
{

	class Model3d
	{
	public:
		Model3d(std::shared_ptr<Mesh> pMesh, std::shared_ptr<Texture2D> pTexture) : m_pModelMesh(pMesh), m_pTexture(pTexture) {}

		std::shared_ptr<Mesh> GetMesh() const { return m_pModelMesh; }
		std::shared_ptr<Texture2D> GetTexture() const { return m_pTexture; }

	private:
		std::shared_ptr<Mesh> m_pModelMesh;
		std::shared_ptr<Texture2D> m_pTexture;
	};

}