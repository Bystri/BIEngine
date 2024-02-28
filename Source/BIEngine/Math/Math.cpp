#include "Math.h"

#include <cmath>

namespace BIEngine {

template <typename T>
T Abs(const T val)
{
   return std::abs(val);
}

template char Abs<char>(const char val);
template int Abs<int>(const int val);
template long Abs<long>(const long val);
template long long Abs<long long>(const long long val);
template float Abs<float>(const float val);
template double Abs<double>(const double val);

float Sqrt(const float val)
{
   return std::sqrt(val);
}

double Sqrt(const double val)
{
   return std::sqrt(val);
}

} // namespace BIEngine
