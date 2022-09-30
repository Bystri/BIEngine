#pragma once

#include "Process.h"

namespace BIEngine
{

	class DelayProcess : public Process
	{
	public:
		explicit DelayProcess(double timeToDelay);

	protected:
		virtual void OnUpdate(double dt) override;

	protected:
		double m_timeToDelay;
		double m_timeDelayedSoFar;
	};

}
