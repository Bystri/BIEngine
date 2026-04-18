#pragma once

#include "../StdLib/SharedPtr.h"
#include "../StdLib/HashMap.h"

namespace BIEngine {

template <class BaseType, class SubType>
SharedPtr<BaseType> GenericObjectCreationFunction()
{
   return SharedPtr<BaseType>(new SubType());
}

template <class BaseClass, class IdType>
class GenericObjectFactory {
   using ObjectCreationFunction = SharedPtr<BaseClass> (*)(void);

public:
   template <class SubClass>
   bool Register(IdType id)
   {
      auto findIt = m_creationFunctions.Find(id);
      if (findIt == m_creationFunctions.End()) {
         m_creationFunctions[id] = &GenericObjectCreationFunction<BaseClass, SubClass>;
         return true;
      }

      return false;
   }

   SharedPtr<BaseClass> Create(IdType id)
   {
      auto findIt = m_creationFunctions.Find(id);
      if (findIt != m_creationFunctions.End()) {
         ObjectCreationFunction pFunc = findIt->second;
         return pFunc();
      }

      return nullptr;
   }

private:
   HashMap<IdType, ObjectCreationFunction> m_creationFunctions;
};

} // namespace BIEngine
