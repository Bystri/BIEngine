#pragma once

#include "Process.h"

namespace BIEngine {

class DelayProcess : public Process {
public:
   explicit DelayProcess(double timeToDelay);

protected:
   virtual void OnUpdate(const GameTimer& gt) override;

protected:
   double m_timeToDelay;
   double m_timeDelayedSoFar;
};

} // namespace BIEngine
