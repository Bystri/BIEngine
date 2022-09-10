#include "DelayProcess.h"

#include <iostream>

namespace BIEngine
{

	DelayProcess::DelayProcess(unsigned long timeToDelay)
		: m_timeToDelay(timeToDelay)
		, m_timeDelayedSoFar(0)
	{
	}

	void DelayProcess::OnUpdate(unsigned long deltaMs)
	{
		m_timeDelayedSoFar += deltaMs;
		if (m_timeDelayedSoFar >= m_timeToDelay)
			Succeed();
	}

}