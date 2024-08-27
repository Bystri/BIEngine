#pragma optimize

#include <unordered_map>
#include <unordered_set>
#include <memory>

#include "Serialization.h"
#include "../EngineCore/Assert.h"

namespace BIEngine {

enum class PacketType {
   Hello,
   ReplicationData,
   RPC,
   Disconnect,
   Max
};

class ReplicationObject {
public:
   static const uint32_t sk_ClassType = 0;

   virtual uint32_t GetClassId() const { return sk_ClassType; }

   static std::shared_ptr<ReplicationObject> CreateInstance() { return nullptr; }

   virtual void Write(OutputMemoryBitStream& stream) = 0;
   virtual void Read(InputMemoryBitStream& stream) = 0;
};

class LinkingContext {
public:
   LinkingContext()
      : m_nextNetworkId(1)
   {
   }

   uint32_t GetNetworkId(std::shared_ptr<ReplicationObject> pGameObject, bool shouldCreateIfNotFound);

   void AddGameObject(std::shared_ptr<ReplicationObject> pGameObject, uint32_t networkId);
   void RemoveGameObject(std::shared_ptr<ReplicationObject> pGameObject);

   std::shared_ptr<ReplicationObject> GetGameObject(uint32_t networkId);

private:
   std::unordered_map<uint32_t, std::shared_ptr<ReplicationObject>> m_networkIdToGameObjectMap;
   std::unordered_map<std::shared_ptr<ReplicationObject>, uint32_t> m_gameObjectToNetworkIdMap;
   uint32_t m_nextNetworkId;
};

using GameObjectCreationFunc = std::shared_ptr<ReplicationObject> (*)();

class ObjectCreationRegistry {
public:
   static ObjectCreationRegistry& Get()
   {
      static ObjectCreationRegistry sInstance;
      return sInstance;
   }

   template <class T>
   void RegisterCreationFunction()
   {
      // ensure no duplicate class id
      Assert(m_nameToGameObjectCreationFunctionMap.find(T::sk_ClassType) == m_nameToGameObjectCreationFunctionMap.end(), "Class type with id %u already registered in ObjectCreationRegistry", T::sk_ClassType);
      m_nameToGameObjectCreationFunctionMap[T::sk_ClassType] = T::CreateInstance;
   }

   std::shared_ptr<ReplicationObject> CreateGameObject(uint32_t classId)
   {
      // add error checking if desired- for now crash if not found
      GameObjectCreationFunc creationFunc = m_nameToGameObjectCreationFunctionMap[classId];
      return creationFunc();
   }

private:
   ObjectCreationRegistry() {}

   std::unordered_map<uint32_t, GameObjectCreationFunc> m_nameToGameObjectCreationFunctionMap;
};

template <int tValue, int tBits>
struct GetRequiredBitsHelper {
   enum {
      Value = GetRequiredBitsHelper<(tValue >> 1), tBits + 1>::Value
   };
};

template <int tBits>
struct GetRequiredBitsHelper<0, tBits> {
   enum { Value = tBits };
};

template <int tValue>
struct GetRequiredBits {
   enum { Value = GetRequiredBitsHelper<tValue, 0>::Value };
};

enum class ReplicationAction {
   Create,
   Update,
   Destroy,
   MAX
};

class ReplicationHeader {
public:
   ReplicationHeader()
      : m_replicationAction(ReplicationAction::Create),
        m_networkId(0),
        m_classId(0)
   {
   }

   ReplicationHeader(ReplicationAction ra, uint32_t networkId, uint32_t classId = 0)
      : m_replicationAction(ra),
        m_networkId(networkId),
        m_classId(classId)
   {
   }

   ReplicationAction GetReplicationAction() const { return m_replicationAction; };

   uint32_t GetNetworkId() const { return m_networkId; }

   uint32_t GetClassId() const { return m_classId; }

   void Write(OutputMemoryBitStream& stream);
   void Read(InputMemoryBitStream& stream);

public:
   ReplicationAction m_replicationAction;
   uint32_t m_networkId;
   uint32_t m_classId;
};

class ReplicationManager {
public:
   ReplicationManager(std::shared_ptr<LinkingContext> pLinkingContext)
      : m_pLinkingContext(pLinkingContext)
   {
   }

public:
   void ReplicateCreate(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject);
   void ReplicateUpdate(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject);
   void ReplicateDestroy(OutputMemoryBitStream& stream, std::shared_ptr<ReplicationObject> pGameObject);

   void ProcessReplicationAction(InputMemoryBitStream& inStream);

private:
   std::shared_ptr<LinkingContext> m_pLinkingContext;
   std::unordered_set<std::shared_ptr<ReplicationObject>> m_objectsReplicatedToMe;
};

using RPCUnwrapFunc = void (*)(InputMemoryBitStream&);

class RPCManager {
public:
   void RegisterUnwrapFunction(uint32_t id, RPCUnwrapFunc func);
   void ProcessRPC(InputMemoryBitStream& stream);

private:
   std::unordered_map<uint32_t, RPCUnwrapFunc> m_nameToRPCTable;
};
} // namespace BIEngine
