#pragma once

namespace BIEngine {

class GameTimer {
public:
   GameTimer();

   float TotalTime() const;
   float DeltaTime() const;

   void Reset(); // Вызывается перед началом цикла отрисовки
   void Start(); // Вызывается во ремя снятия паузы
   void Stop();  // Вызывается во время паузы
   void Tick();  // Вызывается каждый кадр

private:
   double m_deltaTime;

   double m_baseTime;
   double m_pausedTime;
   double m_stopTime;
   double m_prevTime;
   double m_currTime;

   bool m_stopped;
};

} // namespace BIEngine
