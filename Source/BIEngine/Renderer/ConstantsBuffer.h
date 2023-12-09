#pragma once

#include <memory>

#include "Renderer.h"

namespace BIEngine
{

	class ConstantsBuffer
	{
	public:
		ConstantsBuffer();

		void Init(unsigned int bufferSize, unsigned int bindingPoint);

		void SetBufferData(const void* ptrData, unsigned int offset, unsigned int dataSize);

	private:
		unsigned int m_uniformBufferBlock;
		unsigned int m_bufferSize;
	};

}