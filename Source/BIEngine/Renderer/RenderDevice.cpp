#include "RenderDevice.h"

#include <glad/glad.h>

namespace BIEngine
{

    void RenderDevice::SetDepthTest(bool enable)
    {
        if (m_depthTest == enable) {
            return;
        }

        m_depthTest = enable;

        if (enable) {
            glEnable(GL_DEPTH_TEST);
        }
        else {
            glDisable(GL_DEPTH_TEST);
        }
    }


    void RenderDevice::SetDepthFunc(Func depthFunc)
    {
        if (m_depthFunc == depthFunc) {
            return;
        }

        m_depthFunc = depthFunc;
        glDepthFunc(GL_NEVER + static_cast<char>(depthFunc));
    }


    void RenderDevice::SetBlend(bool enable)
    {
        if (m_blend == enable) {
            return;
        }

        m_blend = enable;

        if (enable) {
            glEnable(GL_BLEND);
        }
        else {
            glDisable(GL_BLEND);
        }
    }


    static GLenum renderEnumBlendFuncToGlEnum(RenderDevice::BlendFunc src)
    {
        if (src == RenderDevice::BlendFunc::ZERO || src == RenderDevice::BlendFunc::ONE) {
            return static_cast<int>(src);
        }

        return GL_SRC_COLOR + static_cast<char>(src);
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
        }
        else {
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

}