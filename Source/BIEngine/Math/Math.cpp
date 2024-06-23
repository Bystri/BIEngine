#include "Math.h"

#include <cmath>

namespace BIEngine {

template <typename T>
T Abs(const T& val)
{
   return std::abs(val);
}

template char Abs<char>(const char& val);
template int Abs<int>(const int& val);
template long Abs<long>(const long& val);
template long long Abs<long long>(const long long& val);
template float Abs<float>(const float& val);
template double Abs<double>(const double& val);

float Sqrt(const float val)
{
   return std::sqrt(val);
}

double Sqrt(const double val)
{
   return std::sqrt(val);
}

template <typename T>
extern T Min(const T& a, const T& b)
{
   return a < b ? a : b;
}

template char Min<char>(const char& a, const char& b);
template int Min<int>(const int& a, const int& b);
template long Min<long>(const long& a, const long& b);
template long long Min<long long>(const long long& a, const long long& b);
template float Min<float>(const float& a, const float& b);
template double Min<double>(const double& a, const double& b);

template <typename T>
extern T Max(const T& a, const T& b)
{
   return a > b ? a : b;
}

template char Max<char>(const char& a, const char& b);
template int Max<int>(const int& a, const int& b);
template long Max<long>(const long& a, const long& b);
template long long Max<long long>(const long long& a, const long long& b);
template float Max<float>(const float& a, const float& b);
template double Max<double>(const double& a, const double& b);

template <typename T>
T Clamp(const T& value, const T& min, const T& max)
{
   T ret = Max(value, min);
   return Min(ret, max);
}

template char Clamp<char>(const char& value, const char& min, const char& max);
template int Clamp<int>(const int& value, const int& min, const int& max);
template long Clamp<long>(const long& value, const long& min, const long& max);
template long long Clamp<long long>(const long long& value, const long long& min, const long long& max);
template float Clamp<float>(const float& value, const float& min, const float& max);
template double Clamp<double>(const double& value, const double& min, const double& max);

template <typename T>
T MoveTowards(const T& current, const T& target, const T& maxDelta)
{
   T delta = target - current;
   delta = Min(delta, maxDelta);
   return current + delta;
}

template char MoveTowards<char>(const char& current, const char& target, const char& maxDelta);
template int MoveTowards<int>(const int& current, const int& target, const int& maxDelta);
template long MoveTowards<long>(const long& current, const long& target, const long& maxDelta);
template long long MoveTowards<long long>(const long long& current, const long long& target, const long long& maxDelta);
template float MoveTowards<float>(const float& current, const float& target, const float& maxDelta);
template double MoveTowards<double>(const double& current, const double& target, const double& maxDelta);

template <typename T>
T SmoothDamp(const T& from, const T& to, T& currentVelocity, float smoothTime, float deltaTime, const T& maxSpeed)
{
   // Game Programming Gems 4 Chapter 1.10
   const float omega = 2.0f / smoothTime;
   const float x = omega * deltaTime;
   const float exp = 1.0f / (1.0f + x + 0.48f * x * x + 0.235f * x * x * x);
   T change = from - to;

   const T maxChange = maxSpeed * smoothTime;
   change = Clamp(change, -maxChange, maxChange);
   T target = from - change;

   const T temp = (currentVelocity + omega * change) * deltaTime;
   currentVelocity = (currentVelocity - omega * temp) * exp;
   T ret = target + (change + temp) * exp;

   return ret;
}

template float SmoothDamp<float>(const float& from, const float& to, float& currentVelocity, float smoothTime, float deltaTime, const float& maxSpeed);
template double SmoothDamp<double>(const double& from, const double& to, double& currentVelocity, float smoothTime, float deltaTime, const double& maxSpeed);

} // namespace BIEngine
