#include "ProcessManager.h"

#include <iostream>
#include <cassert>

#include "../Utilities/Logger.h"

namespace BIEngine {

ProcessManager* ProcessManager::s_pSingleton = nullptr;

ProcessManager::ProcessManager() {}

ProcessManager::~ProcessManager()
{
   ClearAllProcesses();
}

bool ProcessManager::Create()
{
   if (s_pSingleton) {
      Logger::WriteLog(Logger::LogType::ERROR, "Overwriting ProcessManager singleton");
      if (s_pSingleton) {
         delete s_pSingleton;
         s_pSingleton = nullptr;
      }
   }

   s_pSingleton = new ProcessManager();
   if (s_pSingleton)
      return true;

   return false;
}

void ProcessManager::Destroy()
{
   assert(s_pSingleton);
   if (s_pSingleton) {
      delete s_pSingleton;
      s_pSingleton = nullptr;
   }
}

unsigned int ProcessManager::UpdateProcesses(double dt)
{
   unsigned short int successCount = 0;
   unsigned short int failCount = 0;

   ProcessList::iterator it = m_processList.begin();
   while (it != m_processList.end()) {
      StrongProcessPtr pCurrProcess = *it;

      if (pCurrProcess->GetState() == Process::State::UNINITIALIZED)
         pCurrProcess->OnInit();

      if (pCurrProcess->GetState() == Process::State::RUNNING)
         pCurrProcess->OnUpdate(dt);

      if (pCurrProcess->IsDead()) {
         switch (pCurrProcess->GetState()) {
            case Process::State::SUCCEEDED:
               {
                  pCurrProcess->OnSuccess();
                  StrongProcessPtr pChild = pCurrProcess->RemoveChild();
                  if (pChild)
                     AttachProcess(pChild);
                  else
                     ++successCount;

                  break;
               }
            case Process::State::FAILED:
               {
                  pCurrProcess->OnFail();
                  ++failCount;
                  break;
               }
            case Process::State::ABORTED:
               {
                  pCurrProcess->OnAbort();
                  ++failCount;
                  break;
               }
            default:
               break;
         }

         it = m_processList.erase(it);
         --it;
      }

      ++it;
   }

   return ((successCount << 16) | failCount);
}

WeakProcessPtr ProcessManager::AttachProcess(StrongProcessPtr pProcess)
{
   m_processList.push_front(pProcess);
   return WeakProcessPtr(pProcess);
}

void ProcessManager::AbortAllProcesses(bool immediate)
{
   for (auto it = m_processList.begin(); it != m_processList.end(); ++it) {
      StrongProcessPtr pProcess = *it;
      if (pProcess->IsAlive()) {
         pProcess->SetState(Process::State::ABORTED);

         if (immediate) {
            pProcess->OnAbort();
            it = m_processList.erase(it);
         }
      }
   }
}

void ProcessManager::ClearAllProcesses()
{
   m_processList.clear();
}

} // namespace BIEngine