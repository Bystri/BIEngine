#pragma optimize

#include "GameTimer.h"

namespace BIEngine {

class TimeWeightedAverage {
public:
   TimeWeightedAverage(const GameTimer& gt, float duration = 1.f) 
       : m_duration(duration)
       , m_value(0.f)
   {
      m_timeLastEntryMade = gt.TotalTime();
   }

   void Update(const GameTimer& gt, float value);

   float GetValue() const { return m_value; }

private:
   float m_timeLastEntryMade;
   float m_value;
   float m_duration;
};

} // namespace BIEngine