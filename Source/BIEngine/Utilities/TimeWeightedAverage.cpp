#include "TimeWeightedAverage.h"

#include "../StdLib/Algorithm.h"

namespace BIEngine {

#pragma optimize("", off)

void TimeWeightedAverage::Update(const GameTimer& gt, float value)
{
   const float time = gt.TotalTime();
   const float timeSinceLastEntry = time - m_timeLastEntryMade;

   // now update our value by whatever amount of the duration that was..
   const float fractionOfDuration = Min(1.0f, (timeSinceLastEntry / m_duration));

   m_value = m_value * (1.f - fractionOfDuration) + value * fractionOfDuration;

   m_timeLastEntryMade = time;
}

} // namespace BIEngine