#include "Audio.h"
#include "SoundLoader.h"

namespace BIEngine {

Audio* g_pAudio = nullptr;
char* gSoundExtentions[] = {".mp3", ".wav", ".midi", ".ogg"};

Audio::Audio()
   : m_initialized(false),
     m_allPaused(false)
{
}

void Audio::Shutdown()
{
   AudioBufferList::Iterator itr = m_allSamples.Begin();

   while (itr != m_allSamples.End()) {
      (*itr)->Stop();
      ++itr;
      m_allSamples.PopFront();
   }
}

void Audio::PauseAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Pause();
   }

   m_allPaused = true;
}

void Audio::ResumeAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Resume();
   }

   m_allPaused = false;
}

void Audio::StopAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Stop();
   }

   m_allPaused = false;
}

bool Audio::HasSoundCard()
{
   return (g_pAudio && g_pAudio->Active());
}

} // namespace BIEngine
