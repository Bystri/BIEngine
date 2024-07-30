#include "Socket.h"

namespace BIEngine {

SocketAddress::SocketAddress()
{
   sockaddr_in* pSocketAddrIn = GetAsSockAddrIn();
   pSocketAddrIn->sin_family = AF_INET;
   pSocketAddrIn->sin_addr.S_un.S_addr = htonl(0xffffffff);
   pSocketAddrIn->sin_port = htons(0);
}

SocketAddress::SocketAddress(uint32_t address, uint16_t port)
{
   sockaddr_in* pSocketAddrIn = GetAsSockAddrIn();
   pSocketAddrIn->sin_family = AF_INET;
   pSocketAddrIn->sin_addr.S_un.S_addr = htonl(address);
   pSocketAddrIn->sin_port = htons(port);
}

SocketAddress::SocketAddress(const sockaddr_storage& sockAddr)
{
   memcpy(&m_sockAddr, &sockAddr, sizeof(sockaddr_storage));
}

/***Socket***/

Socket::~Socket()
{
   closesocket(m_socket);
}

int Socket::Bind(const SocketAddress& sockAddress)
{
   int err = bind(m_socket, sockAddress.GetAsSockAddr(), sockAddress.GetSize());
   if (err != 0) {
      int errCode = SocketUtil::GetLastError();
      Logger::WriteErrorLog("Socket error - Bind; Error code: %d", errCode);
      return errCode;
   }
   return NO_ERROR;
}

/***UdpSocket***/

int UdpSocket::SendTo(const void* data, int len, const SocketAddress& to)
{
   int byteSentCount = sendto(m_socket, static_cast<const char*>(data), len, 0, to.GetAsSockAddr(), to.GetSize());
   if (byteSentCount >= 0) {
      return byteSentCount;
   }

   // return error as negative number
   int errCode = SocketUtil::GetLastError();
   Logger::WriteErrorLog("Udp socket error - SendTo; Error code: %d", errCode);
   return -errCode;
}

int UdpSocket::ReceiveFrom(void* buffer, int maxLen, SocketAddress& from)
{
   int fromLength = from.GetSize();
   int readByteCount = recvfrom(m_socket, static_cast<char*>(buffer), maxLen, 0, from.GetAsSockAddr(), &fromLength);
   if (readByteCount >= 0) {
      return readByteCount;
   }

   int errCode = SocketUtil::GetLastError();
   Logger::WriteErrorLog("Udp socket error - ReceiveFrom; Error code: %d", errCode);
   return -errCode;
}

int UdpSocket::SetNonBlockingMode(bool shouldBeNonBlocking)
{
   u_long arg = shouldBeNonBlocking ? 1 : 0;
   int result = ioctlsocket(m_socket, FIONBIO, &arg);

   if (result == SOCKET_ERROR) {
      int errCode = SocketUtil::GetLastError();
      Logger::WriteErrorLog("Udp socket error - SetNonBlockingMode; Error code: %d", errCode);
      return errCode;
   }

   return NO_ERROR;
}

/***TcpSocket***/

int TcpSocket::Connect(const SocketAddress& address)
{
   int err = connect(m_socket, address.GetAsSockAddr(), address.GetSize());
   if (err < 0) {
      int errorCode = SocketUtil::GetLastError();
      Logger::WriteErrorLog("Tcp socket error - Connect; Error code: %d", errorCode);
      return -errorCode;
   }
   return NO_ERROR;
}

int TcpSocket::Listen(int backLog)
{
   int err = listen(m_socket, backLog);
   if (err < 0) {
      int errorCode = SocketUtil::GetLastError();
      Logger::WriteErrorLog("Tcp socket error - Listen; Error code: %d", errorCode);
      return -errorCode;
   }

   return NO_ERROR;
}

TcpSocketPtr TcpSocket::Accept(SocketAddress& fromAddress)
{
   int length = fromAddress.GetSize();
   SOCKET newSocket = accept(m_socket, fromAddress.GetAsSockAddr(), &length);
   if (newSocket != INVALID_SOCKET) {
      return TcpSocketPtr(new TcpSocket(newSocket));
   }

   int errorCode = SocketUtil::GetLastError();
   Logger::WriteErrorLog("Tcp socket error - Accept; Error code: %d", errorCode);
   return nullptr;
}

int TcpSocket::Send(const void* data, int len)
{
   int bytesSentCount = send(m_socket, static_cast<const char*>(data), len, 0);
   if (bytesSentCount < 0) {
      int errorCode = SocketUtil::GetLastError();
      Logger::WriteErrorLog("Tcp socket error - Send; Error code: %d", errorCode);
      return -errorCode;
   }

   return bytesSentCount;
}

int TcpSocket::Receive(void* buffer, int len)
{
   int bytesReceivedCount = recv(m_socket, static_cast<char*>(buffer), len, 0);
   if (bytesReceivedCount < 0) {
      int errorCode = SocketUtil::GetLastError();
      Logger::WriteErrorLog("Tcp socket error - Receive; Error code: %d", errorCode);
      return -errorCode;
   }
   return bytesReceivedCount;
}

/***SocketUtil***/

bool SocketUtil::Init()
{
   WSADATA lpWSAData;
   return WSAStartup(MAKEWORD(2, 2), &lpWSAData) != NO_ERROR;
}

bool SocketUtil::Terminate()
{
   return WSACleanup() != NO_ERROR;
}

int SocketUtil::GetLastError()
{
   return WSAGetLastError();
}

SocketAddressPtr SocketUtil::CreateIPv4SocketFromString(const std::string& address)
{
   auto pos = address.find_last_of(':');
   std::string host, service;
   if (pos != std::string::npos) {
      host = address.substr(0, pos);
      service = address.substr(pos + 1);
   } else {
      host = address;
      // use default port...
      service = "0";
   }

   addrinfo hint;
   memset(&hint, 0, sizeof(hint));
   hint.ai_family = AF_INET;

   addrinfo* result;
   int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
   if (error != 0 && result != nullptr) {
      freeaddrinfo(result);
      return nullptr;
   }

   while (!result->ai_addr && result->ai_next) {
      result = result->ai_next;
   }

   if (!result->ai_addr) {
      freeaddrinfo(result);
      return nullptr;
   }

   sockaddr_storage* sockAddr = reinterpret_cast<sockaddr_storage*>(result->ai_addr);
   SocketAddressPtr toRet(new SocketAddress(*sockAddr));
   freeaddrinfo(result);
   return toRet;
}

SocketAddressPtr SocketUtil::CreateIPv6SocketFromString(const std::string& address)
{
   auto closedbracket = address.find_last_of(']');

   auto pos = address.find_first_of(':', closedbracket);
   std::string host, service;
   if (pos != std::string::npos) {
      host = address.substr(1, pos - 2);
      service = address.substr(pos + 1);
   } else {
      host = address.substr(1, address.length() - 2);
      // use default port...
      service = "0";
   }

   addrinfo hint;
   memset(&hint, 0, sizeof(hint));
   hint.ai_family = AF_INET6;

   addrinfo* result;
   int error = getaddrinfo(host.c_str(), service.c_str(), &hint, &result);
   if (error != 0 && result != nullptr) {
      freeaddrinfo(result);
      return nullptr;
   }

   while (!result->ai_addr && result->ai_next) {
      result = result->ai_next;
   }

   if (!result->ai_addr) {
      freeaddrinfo(result);
      return nullptr;
   }

   sockaddr_storage* sockAddr = reinterpret_cast<sockaddr_storage*>(result->ai_addr);
   SocketAddressPtr toRet(new SocketAddress(*sockAddr));
   return toRet;
}

UdpSocketPtr SocketUtil::CreateUdpSocket(SocketAddressFamily inFamily)
{
   SOCKET s = socket(static_cast<int>(inFamily), SOCK_DGRAM, IPPROTO_UDP);
   if (s != INVALID_SOCKET) {
      return UdpSocketPtr(new UdpSocket(s));
   }

   Logger::WriteErrorLog("Error while creating udp socket");
   return nullptr;
}

TcpSocketPtr SocketUtil::CreateTcpSocket(SocketAddressFamily inFamily)
{
   SOCKET s = socket(static_cast<int>(inFamily), SOCK_STREAM, IPPROTO_TCP);
   if (s != INVALID_SOCKET) {
      return TcpSocketPtr(new TcpSocket(s));
   }

   Logger::WriteErrorLog("Error while creating udp socket");
   return nullptr;
}

int SocketUtil::Select(const std::vector<TcpSocketPtr>* readSet, std::vector<TcpSocketPtr>* readedSockets, const std::vector<TcpSocketPtr>* writeSet, std::vector<TcpSocketPtr>* writedSockets, const std::vector<TcpSocketPtr>* exceptSet, std::vector<TcpSocketPtr>* exceptedSockets)
{
   // build up some sets from our vectors
   fd_set read, write, except;
   fd_set* readPtr = FillSetFromVector(read, readSet);
   fd_set* writePtr = FillSetFromVector(read, writeSet);
   fd_set* exceptPtr = FillSetFromVector(read, exceptSet);
   int toRet = select(0, readPtr, writePtr, exceptPtr, nullptr);
   if (toRet > 0) {
      FillVectorFromSet(readedSockets, readSet, read);
      FillVectorFromSet(writedSockets, writeSet, write);
      FillVectorFromSet(exceptedSockets, exceptSet, except);
   }
   return toRet;
}

fd_set* SocketUtil::FillSetFromVector(fd_set& set, const std::vector<TcpSocketPtr>* sockets)
{
   if (sockets == nullptr) {
      return nullptr;
   }

   FD_ZERO(&set);
   for (const TcpSocketPtr& socket : *sockets) {
      FD_SET(socket->m_socket, &set);
   }
   return &set;
}

void SocketUtil::FillVectorFromSet(std::vector<TcpSocketPtr>* filledSockets, const std::vector<TcpSocketPtr>* sockets, const fd_set& set)
{
   if (filledSockets == nullptr || sockets == nullptr) {
      return;
   }

   filledSockets->clear();
   for (const TcpSocketPtr& socket : *sockets) {
      if (FD_ISSET(socket->m_socket, &set)) {
         filledSockets->push_back(socket);
      }
   }
}
} // namespace BIEngine
