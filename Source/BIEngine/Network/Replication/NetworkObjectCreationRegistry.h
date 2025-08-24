#pragma once

#include "ObjectReplication.h"
#include "../../Utilities/GenericObjectFactory.h"
#include "../../Utilities/LinkingContext.h"

namespace BIEngine {

using NewtworkObjectLinkingContexts = LinkingContext<ReplicationObject>;
using GameObjectCreationFunc = SharedPtr<ReplicationObject> (*)();

class NetworkObjectCreationRegistry : public GenericObjectFactory<ReplicationObject, uint32_t> {
public:
   static NetworkObjectCreationRegistry& Get()
   {
      static NetworkObjectCreationRegistry sInstance;
      return sInstance;
   }

private:
   NetworkObjectCreationRegistry() {}

   HashMap<uint32_t, GameObjectCreationFunc> m_nameToGameObjectCreationFunctionMap;
};


} // namespace BIEngine
