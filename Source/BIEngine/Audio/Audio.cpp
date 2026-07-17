#include "Audio.h"
#include "SoundLoader.h"

namespace BIEngine {

AudioManager* g_pAudio = nullptr;
char* gSoundExtentions[] = {".mp3", ".wav", ".midi", ".ogg"};

AudioManager::AudioManager()
   : m_initialized(false),
     m_allPaused(false)
{
}

void AudioManager::Shutdown()
{
   AudioBufferList::Iterator itr = m_allSamples.Begin();

   while (itr != m_allSamples.End()) {
      (*itr)->Stop();
      ++itr;
      m_allSamples.PopFront();
   }
}

void AudioManager::PauseAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Pause();
   }

   m_allPaused = true;
}

void AudioManager::ResumeAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Resume();
   }

   m_allPaused = false;
}

void AudioManager::StopAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Stop();
   }

   m_allPaused = false;
}

bool AudioManager::HasSoundCard()
{
   return (g_pAudio && g_pAudio->Active());
}

} // namespace BIEngine
