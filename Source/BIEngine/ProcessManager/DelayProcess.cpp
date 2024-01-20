#include "DelayProcess.h"

#include <iostream>

namespace BIEngine {

DelayProcess::DelayProcess(float timeToDelay)
   : m_timeToDelay(timeToDelay), m_timeDelayedSoFar(0)
{
}

void DelayProcess::OnUpdate(float dt)
{
   m_timeDelayedSoFar += dt;
   if (m_timeDelayedSoFar >= m_timeToDelay) {
      Succeed();
   }
}

} // namespace BIEngine
