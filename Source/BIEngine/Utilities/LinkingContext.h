#pragma once

#include "../StdLib/SharedPtr.h"
#include "../StdLib/HashMap.h"

namespace BIEngine {

template <class ObjectType>
class LinkingContext {

public:
   using IdToObjectMap = HashMap<uint32_t, SharedPtr<ObjectType>>;

public:
   LinkingContext()
      : m_nextId(0u)
   {
   }

   uint32_t GetId(SharedPtr<ObjectType> pObject, bool shouldCreateIfNotFound)
   {
      auto it = m_objectToIdMap.Find(pObject);
      if (it != m_objectToIdMap.End()) {
         return it->second;
      }

      if (shouldCreateIfNotFound) {
         uint32_t newId = m_nextId++;
         AddObj(pObject, newId);
         return newId;
      }

      return 0u;
   }

   void AddObj(SharedPtr<ObjectType> pObject, uint32_t Id)
   {
      m_IdToObjectMap[Id] = pObject;
      m_objectToIdMap[pObject] = Id;
   }

   void RemoveObj(SharedPtr<ObjectType> pObject)
   {
      uint32_t id = m_objectToIdMap[pObject];
      m_objectToIdMap.Erase(pObject);
      m_IdToObjectMap.Erase(id);
   }

   SharedPtr<ObjectType> GetObj(uint32_t id)
   {
      auto it = m_IdToObjectMap.Find(id);
      if (it != m_IdToObjectMap.End()) {
         return it->second;
      }

      return nullptr;
   }

private:
   IdToObjectMap m_IdToObjectMap;
   HashMap<SharedPtr<ObjectType>, uint32_t> m_objectToIdMap;
   uint32_t m_nextId;
};

} // namespace BIEngine
