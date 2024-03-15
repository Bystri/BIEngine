#pragma once

#include "RenderDevice.h"

namespace BIEngine {

struct RenderState {
   bool DepthTest = true;
   bool DepthWrite = true;
   RenderDevice::Func DepthCompare = RenderDevice::Func::LESS;

   bool StencilTest = false;
   bool StencilWrite = false;
   RenderDevice::Func StencilAction;
   int StencilFuncRefValue;
   unsigned int StencilFuncMask;

   bool Cull = true;
   RenderDevice::Face CullFace = RenderDevice::Face::BACK;
   RenderDevice::WindingOrder CullWindingOrder = RenderDevice::WindingOrder::CCW;

   bool Blend = false;
   RenderDevice::BlendFunc BlendSrc = RenderDevice::BlendFunc::ONE;
   RenderDevice::BlendFunc BlendDst = RenderDevice::BlendFunc::ONE_MINUS_SRC_ALPHA;
   RenderDevice::BlendEquation BlendEquation = RenderDevice::BlendEquation::ADD;

   RenderDevice::PolygonMode PolygonMode = RenderDevice::PolygonMode::FILL;
};

} // namespace BIEngine
