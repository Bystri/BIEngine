#include "DelayProcess.h"

#include <iostream>

namespace BIEngine {

DelayProcess::DelayProcess(double timeToDelay)
   : m_timeToDelay(timeToDelay), m_timeDelayedSoFar(0)
{
}

void DelayProcess::OnUpdate(double dt)
{
   m_timeDelayedSoFar += dt;
   if (m_timeDelayedSoFar >= m_timeToDelay)
      Succeed();
}

} // namespace BIEngine