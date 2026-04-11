#pragma once

#include "../StdLib/HashMap.h"
#include "NetworkProtocol.h"

namespace BIEngine {

using RpcId = uint32_t;

using RPCUnwrapFunc = void (*)(InputMemoryBitStream&);

class RpcProtocolWriter : public NetworkProtocolWriter {
public:
   static const NetworkProtocolType sk_ProtocolType;

   static RpcProtocolWriter* Get();

   RpcProtocolWriter();
   virtual ~RpcProtocolWriter();

   void SendRpc(PeerId peerId, RpcId rpcId, const OutputMemoryBitStream& rpcData);

protected:
   virtual const NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

   virtual void RegisterPeer(PeerId peerId) override;
   virtual void UnregisterPeer(PeerId peerId) override;

   virtual void OnBeforePacketsSend(NetworkMessagesManager* pNetworkMessagesManager) override;

private:
   struct PeerInfo {
      PeerId id;
      DynamicArray<OutputMemoryBitStream> m_rpcToSend;
   };
    
   DynamicArray<PeerInfo> m_peerInfos;
};

class RpcProtocolReader : public NetworkProtocolReader {
public:
   static const NetworkProtocolType sk_ProtocolType;

   virtual const NetworkProtocolType& GetType() const override { return sk_ProtocolType; }

   static RpcProtocolReader* Get();

   RpcProtocolReader();
   virtual ~RpcProtocolReader();

   void RegisterUnwrapFunction(RpcId id, RPCUnwrapFunc func);

protected:
   virtual void ReceiveMessage(BIEngine::InputMemoryBitStream& inputStream) override;

private:
   void ProcessRPC(InputMemoryBitStream& stream);

private:
   HashMap<RpcId, RPCUnwrapFunc> m_nameToRPCTable;
};

} // namespace BIEngine
