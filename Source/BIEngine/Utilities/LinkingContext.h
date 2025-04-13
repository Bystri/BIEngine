#pragma once

#include <memory>
#include <unordered_map>

namespace BIEngine {

template <class ObjectType>
class LinkingContext {

public:
   using IdToObjectMap = std::unordered_map<uint32_t, std::shared_ptr<ObjectType>>;

public:
   LinkingContext()
      : m_nextId(0u)
   {
   }

   uint32_t GetId(std::shared_ptr<ObjectType> pObject, bool shouldCreateIfNotFound)
   {
      auto it = m_objectToIdMap.find(pObject);
      if (it != m_objectToIdMap.end()) {
         return it->second;
      }

      if (shouldCreateIfNotFound) {
         uint32_t newId = m_nextId++;
         AddObj(pObject, newId);
         return newId;
      }

      return 0u;
   }

   void AddObj(std::shared_ptr<ObjectType> pObject, uint32_t Id)
   {
      m_IdToObjectMap[Id] = pObject;
      m_objectToIdMap[pObject] = Id;
   }

   void RemoveObj(std::shared_ptr<ObjectType> pObject)
   {
      uint32_t id = m_objectToIdMap[pObject];
      m_objectToIdMap.erase(pObject);
      m_IdToObjectMap.erase(id);
   }

   std::shared_ptr<ObjectType> GetObj(uint32_t id)
   {
      auto it = m_IdToObjectMap.find(id);
      if (it != m_IdToObjectMap.end()) {
         return it->second;
      }

      return nullptr;
   }

private:
   IdToObjectMap m_IdToObjectMap;
   std::unordered_map<std::shared_ptr<ObjectType>, uint32_t> m_objectToIdMap;
   uint32_t m_nextId;
};

} // namespace BIEngine
