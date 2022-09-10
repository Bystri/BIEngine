#include "Material.h"

namespace BIEngine
{

	const float ALPHA_OPAQUE = 1.0f;
	const float ALPHA_TRANSPARENT = 0.0f;

	const Color WHITE = Color(1.0f, 1.0f, 1.0f);
	const Color BLACK = Color(0.0f, 0.0f, 0.0f);
	const Color CYAN = Color(0.0f, 1.0f, 1.0f);
	const Color RED = Color(1.0f, 0.0f, 0.0f);
	const Color GREEN = Color(0.0f, 1.0f, 0.0f);
	const Color BLUE = Color(0.0f, 0.0f, 1.0f);
	const Color YELLOW = Color(1.0f, 1.0f, 0.0f);
	const Color GRAY40 = Color(0.4f, 0.4f, 0.4f);
	const Color GRAY25 = Color(0.25f, 0.25f, 0.25f);
	const Color GRAY65 = Color(0.65f, 0.65f, 0.65f);

	Material::Material()
		: m_Color(WHITE)
		, m_Alpha(ALPHA_OPAQUE)
	{
	}

	void Material::SetColor(const Color& color)
	{
		m_Color = color;
	}

	void Material::SetAlpha(const float alpha)
	{
		m_Alpha = alpha;
	}

}