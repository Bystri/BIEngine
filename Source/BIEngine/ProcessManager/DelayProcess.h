#pragma once

#include "Process.h"

namespace BIEngine
{

	class DelayProcess : public Process
	{
	public:
		explicit DelayProcess(unsigned long timeToDelay);

	protected:
		virtual void OnUpdate(unsigned long deltaMs) override;

	protected:
		unsigned long m_timeToDelay;
		unsigned long m_timeDelayedSoFar;
	};

}
