#pragma once

#include <memory>
#include <unordered_map>

namespace BIEngine {

template <class BaseType, class SubType>
std::shared_ptr<BaseType> GenericObjectCreationFunction()
{
   return std::make_shared<SubType>();
}

template <class BaseClass, class IdType>
class GenericObjectFactory {
   using ObjectCreationFunction = std::shared_ptr<BaseClass> (*)(void);

public:
   template <class SubClass>
   bool Register(IdType id)
   {
      auto findIt = m_creationFunctions.find(id);
      if (findIt == m_creationFunctions.end()) {
         m_creationFunctions[id] = &GenericObjectCreationFunction<BaseClass, SubClass>;
         return true;
      }

      return false;
   }

   std::shared_ptr<BaseClass> Create(IdType id)
   {
      auto findIt = m_creationFunctions.find(id);
      if (findIt != m_creationFunctions.end()) {
         ObjectCreationFunction pFunc = findIt->second;
         return pFunc();
      }

      return nullptr;
   }

private:
   std::unordered_map<IdType, ObjectCreationFunction> m_creationFunctions;
};

} // namespace BIEngine
