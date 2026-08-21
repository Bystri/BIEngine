#include "SoundProcess.h"

#include "Audio.h"
#include "SoundLoader.h"

namespace BIEngine {

SoundProcess::SoundProcess(SharedPtr<ResHandle> pResource, IAudioManager::LoadType loadType, float volume, bool looping)
   : m_pHandle(pResource), m_volume(volume), m_isLooping(looping), m_loadType(loadType)
{
}

SoundProcess::~SoundProcess()
{
   Stop();

   if (m_pAudioBuffer && g_pAudio) {
      g_pAudio->ReleaseAudioBuffer(m_pAudioBuffer);
      m_pAudioBuffer = nullptr;
   }
}

void SoundProcess::OnInit()
{
   Process::OnInit();

   if (m_pHandle == nullptr || g_pAudio == nullptr) {
      Fail();
      return;
   }

   IAudioBuffer* buffer = g_pAudio->InitAudioBuffer(m_pHandle, m_loadType);

   if (!buffer) {
      Fail();
      return;
   }

   m_pAudioBuffer = buffer;

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
   if (!m_pHandle || !m_pAudioBuffer || !m_pAudioSound)
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

void SoundProcess::PauseSound()
{
   if (m_pAudioSound) {
      m_pAudioSound->TogglePause();
   }
}

void SoundProcess::Play(const float volume, const bool looping)
{
   Assert(volume >= 0.0f && volume <= 1.0f, "Volume must be a number between 0.0 and 1.0");

   if (!m_pAudioBuffer || volume < 0.0f || volume > 1.0f) {
      return;
   }

   m_volume = volume;
   m_isLooping = looping;
   m_pAudioSound = m_pAudioBuffer->Play(volume, looping);
   if (m_pAudioSound == nullptr && IsAlive()) {
      Fail();
   }
}

void SoundProcess::Stop()
{
   if (m_pAudioSound) {
      m_pAudioSound->Stop();
      m_pAudioBuffer->ReleaseAudio(m_pAudioSound);
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
