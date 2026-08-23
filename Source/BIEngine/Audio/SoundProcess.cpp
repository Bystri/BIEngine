#include "SoundProcess.h"

#include "Audio.h"
#include "SoundLoader.h"

namespace BIEngine {

SoundProcess::SoundProcess(SharedPtr<ResHandle> pSoundBufferResource, IAudioSound* pSound)
   : m_pHandle(pSoundBufferResource), m_pAudioSound(pSound)
{
}

SoundProcess::~SoundProcess()
{
   Stop();
}

void SoundProcess::OnInit()
{
   Process::OnInit();

   if (m_pHandle == nullptr || g_pAudio == nullptr) {
      Fail();
      return;
   }
}

void SoundProcess::OnUpdate(float dt)
{
   if (!IsPlaying()) {
      Succeed();
   }
}

bool SoundProcess::IsPlaying() const
{
   if (!m_pHandle || !m_pAudioSound)
      return false;

   return m_pAudioSound->IsPlaying();
}

void SoundProcess::SetVolume(float volume)
{
   Assert(volume >= 0 && volume <= 1.0f, "Volume must be a number between 0.0 and 1.0");
   if (volume < 0 || volume > 1.0f) {
      return;
   }

   if (m_pAudioSound) {
      m_pAudioSound->SetVolume(volume);
   }
}

float SoundProcess::GetVolume()
{
   return m_pAudioSound->GetVolume();
}

void SoundProcess::TogglePause()
{
   if (m_pAudioSound) {
      m_pAudioSound->TogglePause();
   }
}

void SoundProcess::Stop()
{
   if (m_pAudioSound) {
      m_pAudioSound->Stop();
   }
}

float SoundProcess::GetProgress() const
{
   if (m_pAudioSound) {
      return m_pAudioSound->GetProgress();
   }

   return 0.0f;
}

} // namespace BIEngine
