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
   AudioBufferList::iterator i = m_allSamples.begin();

   while (i != m_allSamples.end()) {
      IAudioBuffer* audioBuffer = (*i);
      audioBuffer->Stop();
      m_allSamples.pop_front();
   }
}

void Audio::PauseAllSounds()
{
   AudioBufferList::iterator i;
   AudioBufferList::iterator end;
   for (i = m_allSamples.begin(), end = m_allSamples.end(); i != end; ++i) {
      IAudioBuffer* audioBuffer = (*i);
      audioBuffer->Pause();
   }

   m_allPaused = true;
}

void Audio::ResumeAllSounds()
{
   AudioBufferList::iterator i;
   AudioBufferList::iterator end;
   for (i = m_allSamples.begin(), end = m_allSamples.end(); i != end; ++i) {
      IAudioBuffer* audioBuffer = (*i);
      audioBuffer->Resume();
   }

   m_allPaused = false;
}

void Audio::StopAllSounds()
{
   IAudioBuffer* audioBuffer = nullptr;

   AudioBufferList::iterator i;
   AudioBufferList::iterator end;
   for (i = m_allSamples.begin(), end = m_allSamples.end(); i != end; ++i) {
      audioBuffer = (*i);
      audioBuffer->Stop();
   }

   m_allPaused = false;
}

bool Audio::HasSoundCard()
{
   return (g_pAudio && g_pAudio->Active());
}

} // namespace BIEngine
