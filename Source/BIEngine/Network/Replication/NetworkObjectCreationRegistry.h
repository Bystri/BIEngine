#pragma once

#include "ObjectReplication.h"
#include "../../Utilities/GenericObjectFactory.h"
#include "../../Utilities/LinkingContext.h"

namespace BIEngine {

using NewtworkObjectLinkingContexts = LinkingContext<ReplicationObject>;
using GameObjectCreationFunc = std::shared_ptr<ReplicationObject> (*)();

class NetworkObjectCreationRegistry : public GenericObjectFactory<ReplicationObject, uint32_t> {
public:
   static NetworkObjectCreationRegistry& Get()
   {
      static NetworkObjectCreationRegistry sInstance;
      return sInstance;
   }

private:
   NetworkObjectCreationRegistry() {}

   std::unordered_map<uint32_t, GameObjectCreationFunc> m_nameToGameObjectCreationFunctionMap;
};


} // namespace BIEngine
