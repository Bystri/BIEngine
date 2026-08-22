#include "SoundProcess.h"

#include "Audio.h"
#include "SoundLoader.h"

namespace BIEngine {

SoundProcess::SoundProcess(SharedPtr<ResHandle> pResource, float volume, bool looping)
   : m_pHandle(pResource), m_volume(volume), m_isLooping(looping)
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

   Play(m_volume, m_isLooping);
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

   m_volume = volume;

   if (m_pAudioSound) {
      m_pAudioSound->SetVolume(volume);
   }
}

float SoundProcess::GetVolume()
{
   if (m_pAudioSound == nullptr) {
      return m_volume;
   }

   m_volume = m_pAudioSound->GetVolume();
   return m_volume;
}

void SoundProcess::TogglePause()
{
   if (m_pAudioSound) {
      m_pAudioSound->TogglePause();
   }
}

void SoundProcess::Play(const float volume, const bool looping)
{
   Assert(volume >= 0.0f && volume <= 1.0f, "Volume must be a number between 0.0 and 1.0");

   if (volume < 0.0f || volume > 1.0f) {
      return;
   }

   m_volume = volume;
   m_isLooping = looping;
   auto pAudioResExtra = StaticPointerCast<SoundBufferData>(m_pHandle->GetExtra());

   if (pAudioResExtra == nullptr)
   {
       Assert(false, "No extra data for Audio Resource was provided");
       return;
   }

   m_pAudioSound = pAudioResExtra->GetAudioBuffer()->Play(volume, looping);
   if (m_pAudioSound == nullptr && IsAlive()) {
      Fail();
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
