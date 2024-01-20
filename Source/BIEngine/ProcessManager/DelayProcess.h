#pragma once

#include "Process.h"

namespace BIEngine {

class DelayProcess : public Process {
public:
   explicit DelayProcess(float timeToDelay);

protected:
   virtual void OnUpdate(float dt) override;

protected:
   float m_timeToDelay;
   float m_timeDelayedSoFar;
};

} // namespace BIEngine
