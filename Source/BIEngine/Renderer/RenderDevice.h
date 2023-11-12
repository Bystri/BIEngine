#pragma once

namespace BIEngine
{

	class RenderDevice
	{
	public:
		enum class Func : char
		{
			NEVER,
			LESS,
			EQUAL,
			LEQUAL,
			GREATER,
			NOTEQUAL,
			GEQUAL,
			ALWAYS
		};


		enum class BlendFunc : char
		{
			ZERO,
			ONE,
			SRC_COLOR,
			ONE_MINUS_SRC_COLOR,
			SRC_ALPHA,
			ONE_MINUS_SRC_ALPHA,
			DST_ALPHA,
			ONE_MINUS_DST_ALPHA,
			DST_COLOR,
			ONE_MINUS_DST_COLOR,
			SRC_ALPHA_SATURATE
		};


		enum class BlendEquation : char
		{
			ADD,
			REVERSE_SUBTRACT,
			SUBTRACT,
			MIN,
			MAX
		};


		enum class Face : char
		{
			FRONT_LEFT,
			FRONT_RIGHT,
			BACK_LEFT,
			BACK_RIGHT,
			FRONT,
			BACK,
			LEFT,
			RIGHT,
			FRONT_AND_BACK
		};


		enum class WindingOrder : char
		{
			CW,
			CCW
		};


		enum class PolygonMode : char
		{
			POINT,
			LINE,
			FILL
		};

	public:
		RenderDevice()
			: m_depthTest(true)
			, m_depthFunc(Func::LESS)

			, m_blend(false)
			, m_blendSrc(BlendFunc::SRC_ALPHA)
		    , m_blendDst(BlendFunc::ONE_MINUS_SRC_ALPHA)
		    , m_blendEquation(BlendEquation::ADD)

		    , m_cullFace(true)
		    , m_frontFace(Face::BACK)
			, m_windingOrder(WindingOrder::CCW)

			, m_polygonMode(PolygonMode::FILL)
		{}

		void SetDepthTest(bool enable);
		void SetDepthFunc(Func func);
		void SetBlend(bool enable);
		void SetBlendFunc(BlendFunc src, BlendFunc dst);
		void SetBlendEquation(BlendEquation func);
		void SetCull(bool enable);
		void SetCullFace(Face face);
		void SetCullWindingOrder(WindingOrder windingOrder);
		void SetPolygonMode(PolygonMode mode);

	private:
		bool m_depthTest;
		Func m_depthFunc;

		bool m_blend;
		BlendFunc m_blendSrc;
		BlendFunc m_blendDst;
		BlendEquation m_blendEquation;

		bool m_cullFace;
		Face m_frontFace;
		WindingOrder m_windingOrder;
		
		PolygonMode m_polygonMode;
	};

}