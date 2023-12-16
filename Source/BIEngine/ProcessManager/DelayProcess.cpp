#include "DelayProcess.h"

#include <iostream>

namespace BIEngine {

DelayProcess::DelayProcess(double timeToDelay)
   : m_timeToDelay(timeToDelay), m_timeDelayedSoFar(0)
{
}

void DelayProcess::OnUpdate(const GameTimer& gt)
{
   m_timeDelayedSoFar += gt.DeltaTime();
   if (m_timeDelayedSoFar >= m_timeToDelay) {
      Succeed();
   }
}

} // namespace BIEngine
