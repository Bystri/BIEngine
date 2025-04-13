#pragma once

#include <cstring>
#include <cstdlib>
#include <cstdint>
#include <string>
#include <algorithm>
#include <memory>

namespace BIEngine {

template <typename tFrom, typename tTo>
class TypeAliaser {
public:
   TypeAliaser(tFrom inFromValue)
      : mAsFromType(inFromValue) {}

   tTo& Get() { return mAsToType; }

   union {
      tFrom mAsFromType;
      tTo mAsToType;
   };
};

inline uint16_t ByteSwap2(uint16_t inData)
{
   return (inData >> 8) | (inData << 8);
}

inline uint32_t ByteSwap4(uint32_t inData)
{
   return ((inData >> 24) & 0x000000ff) |
          ((inData >> 8) & 0x0000ff00) |
          ((inData << 8) & 0x00ff0000) |
          ((inData << 24) & 0xff000000);
}

inline uint64_t ByteSwap8(uint64_t inData)
{
   return ((inData >> 56) & 0x00000000000000ff) |
          ((inData >> 40) & 0x000000000000ff00) |
          ((inData >> 24) & 0x0000000000ff0000) |
          ((inData >> 8) & 0x00000000ff000000) |
          ((inData << 8) & 0x000000ff00000000) |
          ((inData << 24) & 0x0000ff0000000000) |
          ((inData << 40) & 0x00ff000000000000) |
          ((inData << 56) & 0xff00000000000000);
}

template <typename T, size_t tSize>
class ByteSwapper;

template <typename T>
class ByteSwapper<T, 1> {
public:
   T Swap(T inData) const
   {
      return inData;
   }
};

// specialize for 2...
template <typename T>
class ByteSwapper<T, 2> {
public:
   T Swap(T inData) const
   {
      uint16_t result =
         ByteSwap2(TypeAliaser<T, uint16_t>(inData).Get());
      return TypeAliaser<uint16_t, T>(result).Get();
   }
};

// specialize for 4...
template <typename T>
class ByteSwapper<T, 4> {
public:
   T Swap(T inData) const
   {
      uint32_t result =
         ByteSwap4(TypeAliaser<T, uint32_t>(inData).Get());
      return TypeAliaser<uint32_t, T>(result).Get();
   }
};

// specialize for 8...
template <typename T>
class ByteSwapper<T, 8> {
public:
   T Swap(T inData) const
   {
      uint64_t result =
         ByteSwap8(TypeAliaser<T, uint64_t>(inData).Get());
      return TypeAliaser<uint64_t, T>(result).Get();
   }
};

template <typename T>
T ByteSwap(T inData)
{
   return ByteSwapper<T, sizeof(T)>().Swap(inData);
}

inline bool netIsPlatformLittleEndian()
{
   constexpr uint16_t val = 1;
   char* valPtr = (char*)(&val);
   return valPtr[0] == 1;
}

class OutputMemoryStream {
public:
   OutputMemoryStream()
      : m_pBuffer(nullptr), m_head(0), m_capacity(0)
   {
      reallocBuffer(32);
   }

   // get a pointer to the data in the stream
   const std::shared_ptr<char> GetBufferPtr() const { return m_pBuffer; }

   uint32_t GetLength() const { return m_head; }

   void Write(const void* pData, size_t byteCount);

   template <typename T>
   void Write(T inData)
   {
      static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");
      if (netIsPlatformLittleEndian()) {
         Write(&inData, sizeof(inData));
      } else {
         T swappedData = ByteSwap(inData);
         Write(&swappedData, sizeof(swappedData));
      }
   }

private:
   void reallocBuffer(uint32_t newLength);

private:
   std::shared_ptr<char> m_pBuffer;
   uint32_t m_head;
   uint32_t m_capacity;
};

class InputMemoryStream {
public:
   InputMemoryStream(std::shared_ptr<char> pBuffer, uint32_t byteCount)
      : m_pBuffer(pBuffer), m_head(0), m_capacity(byteCount)
   {
   }

   uint32_t GetRemainingDataSize() const { return m_capacity - m_head; }

   void Read(void* pData, uint32_t byteCount);

   template <typename T>
   void Read(T& inData)
   {
      static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");
      Read(&inData, sizeof(inData));

      if (!netIsPlatformLittleEndian()) {
         inData = ByteSwap(inData);
      }
   }

private:
   std::shared_ptr<char> m_pBuffer;
   uint32_t m_head;
   uint32_t m_capacity;
};

class OutputMemoryBitStream {
public:
   OutputMemoryBitStream() { reallocBuffer(256); }

   std::shared_ptr<char> GetBufferPtr() const { return m_pBuffer; }

   uint32_t GetBitLength() const { return m_bitHead; }

   uint32_t GetByteLength() const { return (m_bitHead + 7) >> 3; }

   void WriteBits(uint8_t data, size_t bitCount);
   void WriteBits(const void* data, size_t bitCount);

   void WriteBytes(const void* data, size_t byteCount) { WriteBits(data, byteCount << 3); }

private:
   void reallocBuffer(uint32_t newBitCapacity);

private:
   std::shared_ptr<char> m_pBuffer;
   uint32_t m_bitHead = 0u;
   uint32_t m_bitCapacity = 0u;
};

class InputMemoryBitStream {
public:
   InputMemoryBitStream(std::shared_ptr<char> pBuffer, uint32_t bitCount)
      : m_pBuffer(pBuffer),
        m_bitCapacity(bitCount),
        m_bitHead(0)
   {
   }

   const std::shared_ptr<char> GetBufferPtr() const { return m_pBuffer; }

   uint32_t GetRemainingBitCount() const { return m_bitCapacity - m_bitHead; }

   void ResetToCapacity(uint32_t byteCapacity)
   {
      m_bitCapacity = byteCapacity << 3;
      m_bitHead = 0;
   }

   void ReadBits(uint8_t& outData, uint32_t bitCount);
   void ReadBits(void* outData, uint32_t bitCount);

   void ReadBytes(void* outData, uint32_t byteCount) { ReadBits(outData, byteCount << 3); }

private:
   std::shared_ptr<char> m_pBuffer;
   uint32_t m_bitHead;
   uint32_t m_bitCapacity;
};

template <typename T>
void Serialize(OutputMemoryBitStream& stream, T data, size_t bitCount = sizeof(T) * 8)
{
   static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");
   if (!netIsPlatformLittleEndian()) {
      data = ByteSwap(data);
   }
   stream.WriteBits(&data, bitCount);
}

void Serialize(OutputMemoryBitStream& stream, const std::string& data);

template <typename T>
void Deserialize(InputMemoryBitStream& stream, T& inData, size_t bitCount = sizeof(T) * 8)
{
   static_assert(std::is_arithmetic<T>::value || std::is_enum<T>::value, "Generic Write only supports primitive data types");
   stream.ReadBits(&inData, bitCount);

   if (!netIsPlatformLittleEndian()) {
      inData = ByteSwap(inData);
   }
}

void Deserialize(InputMemoryBitStream& stream, std::string& data);

} // namespace BIEngine
