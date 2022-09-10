#pragma once

#include <glm/glm.hpp>

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
		Material();

		void SetColor(const Color& color);
		const Color GetColor() { return m_Color; }

		void SetAlpha(const float alpha);
		bool HasAlpha() const { return std::abs(GetAlpha() - ALPHA_OPAQUE) > std::numeric_limits<float>::epsilon(); }
		float GetAlpha() const { return m_Alpha; }

	private:
		Color m_Color;
		float m_Alpha;
	};

}