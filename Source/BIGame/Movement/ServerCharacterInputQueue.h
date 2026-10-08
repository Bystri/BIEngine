#pragma once

#include <deque>

#include "MovementTypes.h"

// Commands received for a player, consumed in fixed-step order on the server
// (and by the owning client's local prediction path).
class ServerCharacterInputQueue {
public:
   void Enqueue(const CharacterInputCommand& command);
   bool PopNext(CharacterInputCommand& command);
   void MarkProcessed(uint32_t sequence);
   void Clear();

   uint32_t GetLastProcessedSequence() const { return m_lastProcessedSequence; }

private:
   std::deque<CharacterInputCommand> m_pending;
   uint32_t m_lastProcessedSequence = 0;
};
