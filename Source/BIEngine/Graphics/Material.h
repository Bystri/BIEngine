#pragma once

#include <glm/glm.hpp>

#include "../Renderer/RenderState.h"
#include "../Renderer/Texture.h"
#include "../Renderer/ShaderProgramState.h"

namespace BIEngine
{

	typedef glm::vec4 Color;

	extern const float ALPHA_OPAQUE;
	extern const float ALPHA_TRANSPARENT;

	extern const Color WHITE;
	extern const Color BLACK;
	extern const Color CYAN;
	extern const Color RED;
	extern const Color GREEN;
	extern const Color BLUE;
	extern const Color YELLOW;
	extern const Color GRAY40;
	extern const Color GRAY25;
	extern const Color GRAY65;

	//Материал отвечает за прозрачность и цвет спрайта
	class Material
	{
	public:
		explicit Material(const std::shared_ptr<ShaderProgram>& pShader);

		void SetColor(const Color& color);
		const Color GetColor() { return m_color; }

		void SetDiffuseMap(std::shared_ptr<Texture2D> diffuseMap) { m_diffuseMap = diffuseMap; }
		const std::shared_ptr<Texture2D>& GetDiffuseMap() const { return m_diffuseMap; }

		void SetSpecularMap(std::shared_ptr<Texture2D> specularMap) { m_specularMap = specularMap; }
		const std::shared_ptr<Texture2D>& GetSpecularMap() const { return m_specularMap; }

		void SetShininess(float shininess) { m_shininess = shininess; }
		float GetShininess() const { return m_shininess; }

		bool HasAlpha() const { return std::abs(m_color.a - ALPHA_OPAQUE) > std::numeric_limits<float>::epsilon(); }

		RenderState& GetRenderState() { return m_renderState; }
		std::shared_ptr<ShaderProgram> GetShaderProgramPtr() { return m_pShaderProgram; }

	private:
		Color m_color;

		std::shared_ptr<Texture2D> m_diffuseMap;
		std::shared_ptr<Texture2D> m_specularMap;
		float m_shininess = 64.f;

		RenderState m_renderState;
		std::shared_ptr<ShaderProgram> m_pShaderProgram;
	};

}