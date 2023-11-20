#pragma once

#include <memory>

#include "Mesh.h"
#include "../Renderer/Texture.h"
#include "MeshGeometryGenerator.h"

namespace BIEngine
{

	class Sprite
	{
	public:
		Sprite(std::shared_ptr<Texture2D> texture) : m_pSpriteMesh(std::make_shared<Mesh>(MeshGeometryGenerator::CreateQuad(1.0, 1.0))), m_pTexture(texture) {}

		std::shared_ptr<Mesh> GetMesh() const { return m_pSpriteMesh; }
		std::shared_ptr<Texture2D> GetTexture() const { return m_pTexture; }

	private:
		std::shared_ptr<Mesh> m_pSpriteMesh;
		std::shared_ptr<Texture2D> m_pTexture;
	};

}