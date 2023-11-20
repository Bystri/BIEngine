#pragma once

#include <glm/glm.hpp>

#include "../Renderer/RenderState.h"
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

		bool HasAlpha() const { return std::abs(m_color.a - ALPHA_OPAQUE) > std::numeric_limits<float>::epsilon(); }

		RenderState& GetRenderState() { return m_renderState; }
		std::shared_ptr<ShaderProgram> GetShaderProgramPtr() { return m_pShaderProgram; }

	private:
		Color m_color;

		RenderState m_renderState;
		std::shared_ptr<ShaderProgram> m_pShaderProgram;
	};

}