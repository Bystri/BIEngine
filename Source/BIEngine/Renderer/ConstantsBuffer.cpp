#include "ConstantsBuffer.h"

#include <glad/glad.h>

namespace BIEngine {

ConstantsBuffer::ConstantsBuffer()
   : m_uniformBufferBlock(0), m_bufferSize(0)
{
}

void ConstantsBuffer::Init(unsigned int bufferSize, unsigned int bindingPoint)
{
   m_bufferSize = bufferSize;

   glGenBuffers(1, &m_uniformBufferBlock);
   glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferBlock);
   glBufferData(GL_UNIFORM_BUFFER, m_bufferSize, nullptr, GL_STATIC_DRAW);
   glBindBuffer(GL_UNIFORM_BUFFER, 0);

   glBindBufferBase(GL_UNIFORM_BUFFER, bindingPoint, m_uniformBufferBlock);
}

void ConstantsBuffer::SetBufferData(const void* ptrData, unsigned int offset, unsigned int dataSize)
{
   assert(offset + dataSize <= m_bufferSize);
   if (offset + dataSize > m_bufferSize) {
      return;
   }

   glBindBuffer(GL_UNIFORM_BUFFER, m_uniformBufferBlock);
   glBufferSubData(GL_UNIFORM_BUFFER, offset, m_bufferSize, ptrData);
   glBindBuffer(GL_UNIFORM_BUFFER, 0);
}
} // namespace BIEngine
