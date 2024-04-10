#include "RenderDevice.h"

#include <glad/glad.h>

#include "../EngineCore/Assert.h"

namespace BIEngine {

static GLenum renderEnumBlendFuncToGlEnum(RenderDevice::BlendFunc src)
{
   if (src == RenderDevice::BlendFunc::ZERO || src == RenderDevice::BlendFunc::ONE) {
      return static_cast<int>(src);
   }

   return GL_SRC_COLOR + static_cast<char>(src);
}

static GLenum renderEnumStencilFuncToGlEnum(RenderDevice::StencilAction action)
{
   switch (action) {
      case RenderDevice::StencilAction::KEEP:
         return GL_KEEP;
      case RenderDevice::StencilAction::ZERO:
         return GL_ZERO;
      case RenderDevice::StencilAction::REPLACE:
         return GL_REPLACE;
      case RenderDevice::StencilAction::INCR:
         return GL_INCR;
      case RenderDevice::StencilAction::INCR_WRAP:
         return GL_INCR_WRAP;
      case RenderDevice::StencilAction::DECR:
         return GL_DECR;
      case RenderDevice::StencilAction::DECR_WRAP:
         return GL_DECR_WRAP;
      case RenderDevice::StencilAction::INVERT:
         return GL_INVERT;
   }

   Assert(false, "RenderDevice::StencilAction is not specified in renderEnumStencilFuncToGlEnum");
   return 0;
}

void RenderDevice::Init()
{
   if (m_depthTest) {
      glEnable(GL_DEPTH_TEST);
   } else {
      glDisable(GL_DEPTH_TEST);
   }


   glDepthMask(m_depthWrite);


   glDepthFunc(GL_NEVER + static_cast<char>(m_depthFunc));

   if (m_stencilTest) {
      glEnable(GL_STENCIL_TEST);
   } else {
      glDisable(GL_STENCIL_TEST);
   }

   glStencilFunc(GL_NEVER + static_cast<char>(m_stencilAction), m_stencilFuncRefValue, m_stencilFuncMask);
   glStencilOp(renderEnumStencilFuncToGlEnum(m_stencilOpFail), renderEnumStencilFuncToGlEnum(m_stencilDpFail), renderEnumStencilFuncToGlEnum(m_stencilDpPass));

   if (m_blend) {
      glEnable(GL_BLEND);
   } else {
      glDisable(GL_BLEND);
   }


   glBlendFunc(renderEnumBlendFuncToGlEnum(m_blendSrc), renderEnumBlendFuncToGlEnum(m_blendDst));


   glBlendEquation(GL_FUNC_ADD + static_cast<char>(m_blendEquation));


   if (m_cullFace) {
      glEnable(GL_CULL_FACE);
   } else {
      glDisable(GL_CULL_FACE);
   }


   glCullFace(GL_FRONT_LEFT + static_cast<char>(m_frontFace));


   glFrontFace(GL_CW + static_cast<char>(m_windingOrder));


   glPolygonMode(GL_FRONT_AND_BACK, GL_FRONT_LEFT + static_cast<char>(m_polygonMode));
}

void RenderDevice::Clear(ClearFlag flags, const ColorRgba& ClearColor)
{
   unsigned int clearBits = 0u;

   if (static_cast<bool>(flags & RenderDevice::ClearFlag::COLOR)) {
      clearBits |= GL_COLOR_BUFFER_BIT;
   }

   if (static_cast<bool>(flags & RenderDevice::ClearFlag::DEPTH)) {
      clearBits |= GL_DEPTH_BUFFER_BIT;
      SetDepthTest(true);
      SetDepthWrite(true);
   }

   if (static_cast<bool>(flags & RenderDevice::ClearFlag::STENCIL)) {
      clearBits |= GL_STENCIL_BUFFER_BIT;
      SetStencilTest(true);
      SetStencilWrite(true);
   }

   glClearColor(ClearColor.r, ClearColor.g, ClearColor.b, ClearColor.a);
   glClear(clearBits);
}

void RenderDevice::SetDepthTest(bool enable)
{
   if (m_depthTest == enable) {
      return;
   }

   m_depthTest = enable;

   if (enable) {
      glEnable(GL_DEPTH_TEST);
   } else {
      glDisable(GL_DEPTH_TEST);
   }
}

void RenderDevice::SetDepthWrite(bool enable)
{
   if (m_depthWrite == enable) {
      return;
   }

   m_depthWrite = enable;
   glDepthMask(m_depthWrite);
}

void RenderDevice::SetDepthFunc(Func depthFunc)
{
   if (m_depthFunc == depthFunc) {
      return;
   }

   m_depthFunc = depthFunc;
   glDepthFunc(GL_NEVER + static_cast<char>(depthFunc));
}

void RenderDevice::SetStencilTest(bool enable)
{
   if (m_stencilTest == enable) {
      return;
   }

   m_stencilTest = enable;

   if (m_stencilTest) {
      glEnable(GL_STENCIL_TEST);
   } else {
      glDisable(GL_STENCIL_TEST);
   }
}

void RenderDevice::SetStencilWrite(bool enable)
{
   if (m_stencilWrite == enable) {
      return;
   }

   m_stencilWrite = enable;

   glStencilMask(m_stencilWrite ? 0xFF : 0x00);
}

void RenderDevice::SetStencilOp(StencilAction fail, StencilAction dpFail, StencilAction dpPass)
{
   if (m_stencilOpFail == fail && m_stencilDpFail == dpFail && m_stencilDpPass == dpPass) {
      return;
   }

   m_stencilOpFail = fail;
   m_stencilDpFail = dpFail;
   m_stencilDpPass = dpPass;

   glStencilOp(renderEnumStencilFuncToGlEnum(m_stencilOpFail), renderEnumStencilFuncToGlEnum(m_stencilDpFail), renderEnumStencilFuncToGlEnum(m_stencilDpPass));
}

void RenderDevice::SetStencilFunc(Func func, int ref, unsigned int mask)
{
   if (m_stencilAction == func && m_stencilFuncRefValue == ref && m_stencilFuncMask == mask) {
      return;
   }

   m_stencilAction = func;
   m_stencilFuncRefValue = ref;
   m_stencilFuncMask = mask;

   glStencilFunc(GL_NEVER + static_cast<char>(m_stencilAction), m_stencilFuncRefValue, m_stencilFuncMask);
}

void RenderDevice::SetBlend(bool enable)
{
   if (m_blend == enable) {
      return;
   }

   m_blend = enable;

   if (enable) {
      glEnable(GL_BLEND);
   } else {
      glDisable(GL_BLEND);
   }
}

void RenderDevice::SetBlendFunc(BlendFunc src, BlendFunc dst)
{
   if (m_blendSrc == src && m_blendDst == dst) {
      return;
   }

   m_blendSrc = src;
   m_blendDst = dst;

   glBlendFunc(renderEnumBlendFuncToGlEnum(src), renderEnumBlendFuncToGlEnum(dst));
}

void RenderDevice::SetBlendEquation(BlendEquation func)
{
   if (m_blendEquation == func) {
      return;
   }

   m_blendEquation = func;
   glBlendEquation(GL_FUNC_ADD + static_cast<char>(func));
}

void RenderDevice::SetCull(bool enable)
{
   if (m_cullFace == enable) {
      return;
   }

   m_cullFace = enable;

   if (enable) {
      glEnable(GL_CULL_FACE);
   } else {
      glDisable(GL_CULL_FACE);
   }
}

void RenderDevice::SetCullFace(Face face)
{
   if (m_frontFace == face) {
      return;
   }

   m_frontFace = face;
   glCullFace(GL_FRONT_LEFT + static_cast<char>(face));
}

void RenderDevice::SetCullWindingOrder(WindingOrder windingOrder)
{
   if (m_windingOrder == windingOrder) {
      return;
   }

   m_windingOrder = windingOrder;
   glFrontFace(GL_CW + static_cast<char>(windingOrder));
}

void RenderDevice::SetPolygonMode(PolygonMode mode)
{
   if (m_polygonMode == mode) {
      return;
   }

   m_polygonMode = mode;
   glPolygonMode(GL_FRONT_AND_BACK, GL_FRONT_LEFT + static_cast<char>(mode));
}

} // namespace BIEngine
