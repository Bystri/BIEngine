#include "GameTimer.h"

#include <GLFW/glfw3.h>

namespace BIEngine {

GameTimer::GameTimer()
   : m_deltaTime(-1.0), m_baseTime(0), m_pausedTime(0), m_prevTime(0), m_currTime(0), m_stopped(false)
{
}

float GameTimer::TotalTime() const
{
   if (m_stopped) {
      return (float)(((m_stopTime - m_pausedTime) - m_baseTime));
   }

   return (float)((m_currTime - m_pausedTime) - m_baseTime);
}

float GameTimer::DeltaTime() const
{
   return (float)m_deltaTime;
}

void GameTimer::Reset()
{
   const double currTime = glfwGetTime();

   m_baseTime = currTime;
   m_prevTime = currTime;
   m_stopTime = 0;
   m_stopped = false;
}

void GameTimer::Start()
{
   const double startTime = glfwGetTime();

   if (m_stopped) {
      m_pausedTime += (startTime - m_stopTime);

      m_prevTime = startTime;
      m_stopTime = 0;
      m_stopped = false;
   }
}

void GameTimer::Stop()
{
   if (!m_stopped) {
      m_stopTime = glfwGetTime();
      m_stopped = true;
   }
}

void GameTimer::Tick()
{
   if (m_stopped) {
      m_deltaTime = 0.0;
      return;
   }

   const double currTime = glfwGetTime();

   m_currTime = currTime;
   m_deltaTime = (m_currTime - m_prevTime);
   m_prevTime = m_currTime;

   if (m_deltaTime < 0.0) {
      m_deltaTime = 0.0;
   }
}

} // namespace BIEngine
