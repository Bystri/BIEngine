#include "ServerCharacterInputQueue.h"

void ServerCharacterInputQueue::Enqueue(const CharacterInputCommand& command)
{
   m_pending.push_back(command);
}

bool ServerCharacterInputQueue::PopNext(CharacterInputCommand& command)
{
   if (m_pending.empty()) {
      return false;
   }
   command = m_pending.front();
   m_pending.pop_front();
   return true;
}

void ServerCharacterInputQueue::MarkProcessed(uint32_t sequence)
{
   m_lastProcessedSequence = sequence;
}

void ServerCharacterInputQueue::Clear()
{
   m_pending.clear();
}
