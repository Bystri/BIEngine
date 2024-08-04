#include "Serialization.h"

#include "../Utilities/Logger.h"

namespace BIEngine {

/***OutputMemoryStream***/

void OutputMemoryStream::reallocBuffer(uint32_t newLength)
{
   char* const pNewBuffer = static_cast<char*>(std::malloc(newLength));

   if (pNewBuffer == nullptr) {
      Logger::WriteErrorLog("Error while allocating buffer of size %d in OutputMemoryStream", newLength);
      return;
   }

   std::memset(pNewBuffer, 0, newLength);
   std::memcpy(pNewBuffer, m_pBuffer.get(), m_capacity);
   m_pBuffer = std::shared_ptr<char>(pNewBuffer, std::free);
   m_capacity = newLength;
}

void OutputMemoryStream::Write(const void* pData, size_t byteCount)
{
   // make sure we have space...
   const uint32_t resultHead = m_head + static_cast<uint32_t>(byteCount);
   if (resultHead > m_capacity) {
      reallocBuffer(std::max(m_capacity * 2, resultHead));
   }
   // copy into buffer at head
   std::memcpy(m_pBuffer.get() + m_head, pData, byteCount);
   // increment head for next write
   m_head = resultHead;
}

/***InputMemoryStream***/

void InputMemoryStream::Read(void* outData, uint32_t byteCount)
{
   const uint32_t resultHead = m_head + byteCount;
   if (resultHead > m_capacity) {
      Logger::WriteErrorLog("Error while reading buffer of InputMemoryStream; Buffer doesn't fit necessary amount of data");
      return;
   }

   std::memcpy(outData, m_pBuffer.get() + m_head, byteCount);

   m_head = resultHead;
}

/***OutputMemoryBitStream***/

void OutputMemoryBitStream::reallocBuffer(uint32_t newBitLength)
{
   const uint32_t newByteLength = newBitLength >> 3;

   char* const pNewBuffer = static_cast<char*>(std::malloc(newByteLength));

   if (pNewBuffer == nullptr) {
      Logger::WriteErrorLog("Error while allocating buffer of size %d in OutputMemoryBitStream", newByteLength);
      return;
   }

   std::memset(pNewBuffer, 0, newByteLength);
   std::memcpy(pNewBuffer, m_pBuffer.get(), m_bitCapacity >> 3);
   m_pBuffer = std::shared_ptr<char>(pNewBuffer, std::free);

   m_bitCapacity = newBitLength;
}

void OutputMemoryBitStream::WriteBits(uint8_t data, size_t bitCount)
{
   const uint32_t nextBitHead = m_bitHead + static_cast<uint32_t>(bitCount);
   if (nextBitHead > m_bitCapacity) {
      reallocBuffer(std::max(m_bitCapacity * 2, nextBitHead));
   }
   // calculate the byteOffset into our buffer
   // by dividing the head by 8
   // and the bitOffset by taking the last 3 bits
   const uint32_t byteOffset = m_bitHead >> 3;
   const uint32_t bitOffset = m_bitHead & 0x7;
   // calculate which bits of the current byte to preserve
   const uint8_t currentMask = ~(0xff << bitOffset);

   char* const pBuffer = m_pBuffer.get();
   pBuffer[byteOffset] = (pBuffer[byteOffset] & currentMask) | (data << bitOffset);
   // calculate how many bits were not yet used in
   // our target byte in the buffer
   uint32_t bitsFreeThisByte = 8 - bitOffset;
   // if we needed more than that, carry to the next byte
   if (bitsFreeThisByte < bitCount) {
      // we need another byte
      pBuffer[byteOffset + 1] = data >> bitsFreeThisByte;
   }
   m_bitHead = nextBitHead;
}

void OutputMemoryBitStream::WriteBits(const void* data, size_t bitCount)
{
   const char* srcByte = static_cast<const char*>(data);

   // write all the bytes
   while (bitCount > 8) {
      WriteBits(*srcByte, 8);
      ++srcByte;
      bitCount -= 8;
   }
   // write anything left
   if (bitCount > 0) {
      WriteBits(*srcByte, bitCount);
   }
}

/***InputMemoryBitStream***/

void InputMemoryBitStream::ReadBits(uint8_t& outData, uint32_t bitCount)
{
   const uint32_t byteOffset = m_bitHead >> 3;
   const uint32_t bitOffset = m_bitHead & 0x7;

   const char* const pBuffer = m_pBuffer.get();
   outData = static_cast<uint8_t>(pBuffer[byteOffset]) >> bitOffset;

   const uint32_t bitsFreeThisByte = 8 - bitOffset;
   if (bitsFreeThisByte < bitCount) {
      // we need another byte
      outData |= static_cast<uint8_t>(pBuffer[byteOffset + 1]) << bitsFreeThisByte;
   }

   // don't forget a mask so that we only read the bit we wanted...
   outData &= (~(0x00ff << bitCount));

   m_bitHead += bitCount;
}

void InputMemoryBitStream::ReadBits(void* outData, uint32_t bitCount)
{
   uint8_t* destByte = reinterpret_cast<uint8_t*>(outData);
   // write all the bytes
   while (bitCount > 8) {
      ReadBits(*destByte, 8);
      ++destByte;
      bitCount -= 8;
   }
   // write anything left
   if (bitCount > 0) {
      ReadBits(*destByte, bitCount);
   }
}

} // namespace BIEngine
