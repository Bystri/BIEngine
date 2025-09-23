#include "Process.h"

namespace BIEngine {

Process::~Process()
{
   if (m_pChild) {
      m_pChild->OnAbort();
   }
}

StrongProcessPtr Process::RemoveChild()
{
   if (m_pChild) {
      StrongProcessPtr pChild = m_pChild;
      m_pChild.Reset();

      return pChild;
   }

   return StrongProcessPtr();
}

} // namespace BIEngine
