#pragma once

#include <list>

#include "Process.h"

namespace BIEngine {

// Данный класс реализован за счет шаблона "одиночка" и доступ к нему может быть получен через вызов  ProcessManager::Get()
// Перед использованием класса требуется предварительная инициализация через Create()
class ProcessManager {
   typedef std::list<StrongProcessPtr> ProcessList;
   ProcessList m_processList;

   static ProcessManager* s_pSingleton;

public:
   static bool Create();
   static void Destroy();

   static ProcessManager* Get()
   {
      Assert(s_pSingleton, "Call ProcessManager::Create before ProcessManager::Get");
      return s_pSingleton;
   }

   // Выполняет один тик работы для всех активных процессов
   unsigned int UpdateProcesses(const GameTimer& gt);

   WeakProcessPtr AttachProcess(StrongProcessPtr pProcess);
   void AbortAllProcesses(bool immediate);

   std::size_t GetProcessCount() const { return m_processList.size(); }

private:
   void ClearAllProcesses();

   explicit ProcessManager();
   ~ProcessManager();
};

} // namespace BIEngine
