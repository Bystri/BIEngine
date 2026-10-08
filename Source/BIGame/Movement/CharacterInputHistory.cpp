#include "CharacterInputHistory.h"

void CharacterInputHistory::Record(const CharacterInputCommand& command)
{
   m_pending.push_back(command);
   m_lastRecordedSequence = command.sequence;
}

bool CharacterInputHistory::Acknowledge(uint32_t lastProcessedSequence)
{
   if (lastProcessedSequence < m_lastAcknowledgedSequence ||
       lastProcessedSequence > m_lastRecordedSequence) {
      return false;
   }

   m_lastAcknowledgedSequence = lastProcessedSequence;
   while (!m_pending.empty() && m_pending.front().sequence <= lastProcessedSequence) {
      m_pending.pop_front();
   }
   return true;
}

void CharacterInputHistory::Clear()
{
   m_pending.clear();
   m_lastRecordedSequence = 0;
   m_lastAcknowledgedSequence = 0;
}
