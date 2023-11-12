#pragma once

#include <glm/glm.hpp>

#include "../Renderer/RenderState.h"
#include "../Renderer/ShaderProgramState.h"

namespace BIEngine
{

	typedef glm::vec3 Color;

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

		void SetAlpha(const float alpha);
		bool HasAlpha() const { return std::abs(GetAlpha() - ALPHA_OPAQUE) > std::numeric_limits<float>::epsilon(); }
		float GetAlpha() const { return m_alpha; }

		RenderState& GetRenderState() { return m_renderState; }
		ShaderProgramState& GetShaderProgramState() { return m_shaderProgramState; }

	private:
		Color m_color;
		float m_alpha;

		RenderState m_renderState;
		ShaderProgramState m_shaderProgramState;
	};

}