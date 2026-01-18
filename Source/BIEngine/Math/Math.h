#pragma once

namespace BIEngine {

template <typename T>
extern T Abs(const T& val);

float Sqrt(const float val);
double Sqrt(const double val);

template <typename T>
extern T Clamp(const T& value, const T& min, const T& max);

template <typename T>
extern T MoveTowards(const T& current, const T& target, const T& maxDelta);

template <typename T>
extern T SmoothDamp(const T& current, const T& target, T& currentVelocity, float smoothTime, float deltaTime, const T& maxSpeed);

} // namespace BIEngine
