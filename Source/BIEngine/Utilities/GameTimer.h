#pragma once

namespace BIEngine {

class GameTimer {
public:
   GameTimer();

   float TotalTime() const;
   float DeltaTime() const;

   void Reset(); // ���������� ����� ������� ����� ���������
   void Start(); // ���������� �� ���� ������ �����
   void Stop();  // ���������� �� ����� �����
   void Tick();  // ���������� ������ ����

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
