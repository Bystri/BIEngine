#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../Utilities/Logger.h"

#include <WinSock2.h>
#include <WS2tcpip.h>

namespace BIEngine {

enum class SocketAddressFamily {
   INET = AF_INET,
   INET6 = AF_INET6
};

class SocketAddress {
   friend class Socket;
   friend class TcpSocket;
   friend class UdpSocket;
   friend class SocketUtil;

public:
   SocketAddress();
   SocketAddress(uint32_t address, uint16_t port);

   uint32_t GetAddress() const
   {
      const sockaddr_in* const pSocketAddrIn = GetAsSockAddrIn();
      return pSocketAddrIn->sin_addr.S_un.S_addr;
   }

   uint16_t GetPort() const
   {
      const sockaddr_in* const pSocketAddrIn = GetAsSockAddrIn();
      return pSocketAddrIn->sin_port;
   }

   bool operator==(const SocketAddress& inOther) const
   {
      return (m_sockAddr.ss_family == AF_INET &&
              GetAsSockAddrIn()->sin_port == inOther.GetAsSockAddrIn()->sin_port) &&
             (GetAsSockAddrIn()->sin_addr.S_un.S_addr == inOther.GetAsSockAddrIn()->sin_addr.S_un.S_addr);
   }

   size_t GetHash() const
   {
      return (GetAsSockAddrIn()->sin_addr.S_un.S_addr) |
             ((static_cast<uint32_t>(GetAsSockAddrIn()->sin_port)) << 13) |
             m_sockAddr.ss_family;
   }


private:
   SocketAddress(const sockaddr_storage& sockAddr);

   sockaddr* GetAsSockAddr()
   {
      return reinterpret_cast<sockaddr*>(&m_sockAddr);
   }

   const sockaddr* GetAsSockAddr() const
   {
      return reinterpret_cast<const sockaddr*>(&m_sockAddr);
   }

   sockaddr_in* GetAsSockAddrIn()
   {
      return reinterpret_cast<sockaddr_in*>(&m_sockAddr);
   }

   const sockaddr_in* GetAsSockAddrIn() const
   {
      return reinterpret_cast<const sockaddr_in*>(&m_sockAddr);
   }

   std::size_t GetSize() const { return sizeof(m_sockAddr); }

private:
   sockaddr_storage m_sockAddr;
};

using SocketAddressPtr = std::shared_ptr<SocketAddress>;

class Socket {
   friend class SocketUtil;

public:
   virtual ~Socket();

   int Bind(const SocketAddress& sockAddress);

protected:
   Socket(SOCKET socket)
      : m_socket(socket)
   {
   }

protected:
   SOCKET m_socket;
};

class UdpSocket : public Socket {
   friend class SocketUtil;

public:
   int SendTo(const void* data, int len, const SocketAddress& to);
   int ReceiveFrom(void* buffer, int maxLen, SocketAddress& from);
   int SetNonBlockingMode(bool shouldBeNonBlocking);

protected:
   UdpSocket(SOCKET socket)
      : Socket(socket)
   {
   }
};

using UdpSocketPtr = std::shared_ptr<UdpSocket>;

class TcpSocket;

using TcpSocketPtr = std::shared_ptr<TcpSocket>;

class TcpSocket : public Socket {
   friend class SocketUtil;

public:
   int Connect(const SocketAddress& address);
   int Listen(int backLog = 32);

   TcpSocketPtr Accept(SocketAddress& fromAddress);

   int Send(const void* data, int len);
   int Receive(void* buffer, int len);

protected:
   TcpSocket(SOCKET socket)
      : Socket(socket)
   {
   }
};

class SocketUtil {
public:
   static bool Init();
   static bool Terminate();

   static int GetLastError();

   static SocketAddressPtr CreateIPv4SocketFromString(const std::string& address);
   static SocketAddressPtr CreateIPv6SocketFromString(const std::string& address);


   static UdpSocketPtr CreateUdpSocket(SocketAddressFamily inFamily);
   static TcpSocketPtr CreateTcpSocket(SocketAddressFamily inFamily);

   static int Select(const std::vector<TcpSocketPtr>* readSet, std::vector<TcpSocketPtr>* readedSockets, const std::vector<TcpSocketPtr>* writeSet, std::vector<TcpSocketPtr>* writedSockets, const std::vector<TcpSocketPtr>* exceptSet, std::vector<TcpSocketPtr>* exceptedSockets);

private:
   static fd_set* FillSetFromVector(fd_set& set, const std::vector<TcpSocketPtr>* sockets);
   static void FillVectorFromSet(std::vector<TcpSocketPtr>* filledSockets, const std::vector<TcpSocketPtr>* sockets, const fd_set& set);
};

} // namespace BIEngine

namespace std {
template <>
struct hash<BIEngine::SocketAddress> {
   size_t operator()(const BIEngine::SocketAddress& inAddress) const
   {
      return inAddress.GetHash();
   }
};
} // namespace std
