#pragma once

#include <memory>
#include <cassert>

#include "../Utilities/Logger.h"

namespace BIEngine {

class Process;
typedef std::shared_ptr<Process> StrongProcessPtr;
typedef std::weak_ptr<Process> WeakProcessPtr;

class Process {
   friend class ProcessManager;

public:
   enum class State {
      // Создан, но не запущен
      UNINITIALIZED = 0,
      // Удален из списка процессов, но не уничтожен
      REMOVED,

      // Запущенный процесс
      RUNNING,
      // Приостановленный процесс
      PAUSED,

      // Успешно завершенный процесс
      SUCCEEDED,
      // Процесс не удалось завершить
      FAILED,
      // Отмененый процесс
      ABORTED
   };

private:
   State m_state;
   StrongProcessPtr m_pChild;

public:
   Process();
   virtual ~Process();

protected:
   virtual void OnInit() { m_state = State::RUNNING; }

   virtual void OnUpdate(double dt) = 0;

   virtual void OnSuccess() {}

   virtual void OnFail() {}

   virtual void OnAbort() {}

public:
   void Succeed();
   void Fail();

   void Pause();
   void UnPause();

   State GetState() const { return m_state; }

   bool IsAlive() const { return (m_state == State::RUNNING || m_state == State::PAUSED); }

   bool IsDead() const { return (m_state == State::SUCCEEDED || m_state == State::FAILED || m_state == State::ABORTED); }

   bool IsRemoved() const { return m_state == State::REMOVED; }

   bool IsPaused() const { return m_state == State::PAUSED; }

   void AttachChild(StrongProcessPtr pChild);
   StrongProcessPtr RemoveChild();

   StrongProcessPtr PeekChild() { return m_pChild; }

private:
   void SetState(State newState) { m_state = newState; }
};

inline void Process::Succeed()
{
   assert(m_state == State::RUNNING || m_state == State::PAUSED);

   m_state = State::SUCCEEDED;
}

inline void Process::Fail()
{
   assert(m_state == State::RUNNING || m_state == State::PAUSED);

   m_state = State::FAILED;
}

inline void Process::Pause()
{
   if (m_state == State::RUNNING)
      m_state = State::PAUSED;
   else
      Logger::WriteLog(Logger::LogType::WARNING, "Try to pause innactive process");
}

inline void Process::UnPause()
{
   if (m_state == State::PAUSED)
      m_state = State::RUNNING;
   else
      Logger::WriteLog(Logger::LogType::WARNING, "Try to unpause active process");
}

inline void Process::AttachChild(StrongProcessPtr pChild)
{
   if (m_pChild)
      m_pChild->AttachChild(pChild);
   else
      m_pChild = pChild;
}

} // namespace BIEngine
