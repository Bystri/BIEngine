#pragma once

#include <type_traits>

#include <glm/glm.hpp>

#include "Color.h"

namespace BIEngine {

class RenderDevice {
public:
   enum class ClearFlag : unsigned char {
      NONE,
      COLOR = 1,
      DEPTH = 1 << 1,
      STENCIL = 1 << 2,
      ALL = COLOR | DEPTH | STENCIL
   };

   enum class StencilAction : char {
      KEEP,
      ZERO,
      REPLACE,
      INCR,
      INCR_WRAP,
      DECR,
      DECR_WRAP,
      INVERT
   };


   enum class Func : char {
      NEVER,
      LESS,
      EQUAL,
      LEQUAL,
      GREATER,
      NOTEQUAL,
      GEQUAL,
      ALWAYS
   };


   enum class BlendFunc : char {
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


   enum class BlendEquation : char {
      ADD,
      REVERSE_SUBTRACT,
      SUBTRACT,
      MIN,
      MAX
   };


   enum class Face : char {
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


   enum class WindingOrder : char {
      CW,
      CCW
   };


   enum class PolygonMode : char {
      POINT,
      LINE,
      FILL
   };

public:
   RenderDevice()
      : m_depthTest(true), m_depthWrite(true), m_depthFunc(Func::LESS),
        m_stencilWrite(false), m_stencilOpFail(StencilAction::KEEP), m_stencilDpFail(StencilAction::KEEP), m_stencilDpPass(StencilAction::KEEP),
        m_stencilAction(Func::ALWAYS), m_stencilFuncRefValue(1), m_stencilFuncMask(0xFF),
        m_blend(false), m_blendSrc(BlendFunc::SRC_ALPHA), m_blendDst(BlendFunc::ONE_MINUS_SRC_ALPHA), m_blendEquation(BlendEquation::ADD),
        m_cullFace(true), m_frontFace(Face::BACK), m_windingOrder(WindingOrder::CCW),
        m_polygonMode(PolygonMode::FILL)
   {
   }

   void Init();

   void Clear(ClearFlag flags, const ColorRgba& color);

   void SetDepthTest(bool enable);
   void SetDepthWrite(bool enable);
   void SetDepthFunc(Func func);
   void SetStencilTest(bool enable);
   void SetStencilWrite(bool enable);
   void SetStencilOp(StencilAction fail, StencilAction dpFail, StencilAction dpPass);
   void SetStencilFunc(Func func, int ref, unsigned int mask);
   void SetBlend(bool enable);
   void SetBlendFunc(BlendFunc src, BlendFunc dst);
   void SetBlendEquation(BlendEquation func);
   void SetCull(bool enable);
   void SetCullFace(Face face);
   void SetCullWindingOrder(WindingOrder windingOrder);
   void SetPolygonMode(PolygonMode mode);

private:
   bool m_depthTest;
   bool m_depthWrite;
   Func m_depthFunc;

   bool m_stencilTest;
   bool m_stencilWrite;
   StencilAction m_stencilOpFail;
   StencilAction m_stencilDpFail;
   StencilAction m_stencilDpPass;
   Func m_stencilAction;
   int m_stencilFuncRefValue;
   unsigned int m_stencilFuncMask;

   bool m_blend;
   BlendFunc m_blendSrc;
   BlendFunc m_blendDst;
   BlendEquation m_blendEquation;

   bool m_cullFace;
   Face m_frontFace;
   WindingOrder m_windingOrder;

   PolygonMode m_polygonMode;
};

inline RenderDevice::ClearFlag operator|(RenderDevice::ClearFlag lhs, RenderDevice::ClearFlag rhs)
{
   using T = std::underlying_type_t<RenderDevice::ClearFlag>;
   return static_cast<RenderDevice::ClearFlag>(static_cast<T>(lhs) | static_cast<T>(rhs));
}

inline RenderDevice::ClearFlag& operator|=(RenderDevice::ClearFlag& lhs, RenderDevice::ClearFlag rhs)
{
   lhs = lhs | rhs;
   return lhs;
}

inline RenderDevice::ClearFlag operator&(RenderDevice::ClearFlag lhs, RenderDevice::ClearFlag rhs)
{
   using T = std::underlying_type_t<RenderDevice::ClearFlag>;
   return static_cast<RenderDevice::ClearFlag>(static_cast<T>(lhs) & static_cast<T>(rhs));
}

inline RenderDevice::ClearFlag& operator&=(RenderDevice::ClearFlag& lhs, RenderDevice::ClearFlag rhs)
{
   lhs = lhs & rhs;
   return lhs;
}

} // namespace BIEngine
