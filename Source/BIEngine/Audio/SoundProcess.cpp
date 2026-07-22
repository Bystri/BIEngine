#include "SoundProcess.h"

#include "Audio.h"
#include "SoundLoader.h"

namespace BIEngine {

SoundProcess::SoundProcess(SharedPtr<ResHandle> pResource, IAudioManager::LoadType loadType, int volume, bool looping)
   : m_pHandle(pResource), m_volume(volume), m_isLooping(looping), m_loadType(loadType)
{
}

SoundProcess::~SoundProcess()
{
   if (IsPlaying())
      Stop();

   if (m_pAudioBuffer)
      g_pAudio->ReleaseAudioBuffer(m_pAudioBuffer.Get());
}

int SoundProcess::GetLengthMilli()
{
   return 0;
}

void SoundProcess::OnInit()
{
   Process::OnInit();

   if (m_pHandle == nullptr)
      return;

   IAudioBuffer* buffer = g_pAudio->InitAudioBuffer(m_pHandle, m_loadType);

   if (!buffer) {
      Fail();
      return;
   }

   m_pAudioBuffer.Reset(buffer);

   Play(m_volume, m_isLooping);
}

void SoundProcess::OnUpdate(float dt)
{
   if (!IsPlaying()) {
      Succeed();
   }
}

bool SoundProcess::IsPlaying()
{
   if (!m_pHandle || !m_pAudioBuffer)
      return false;

   return m_pAudioBuffer->IsPlaying();
}

void SoundProcess::SetVolume(int volume)
{
   if (m_pAudioBuffer == nullptr) {
      return;
   }

   Assert(volume >= 0 && volume <= 100, "Volume must be a number between 0 and 100");
   if (volume < 0 || volume > 100) {
      return;
   }

   m_volume = volume;
   m_pAudioBuffer->SetVolume(volume);
}

int SoundProcess::GetVolume()
{
   if (m_pAudioBuffer == nullptr) {
      return 0;
   }

   m_volume = m_pAudioBuffer->GetVolume();
   return m_volume;
}

void SoundProcess::PauseSound()
{
   if (m_pAudioBuffer)
      m_pAudioBuffer->TogglePause();
}

void SoundProcess::Play(const int volume, const bool looping)
{
   Assert(volume >= 0 && volume <= 100, "Volume must be a number between 0 and 100");

   if (!m_pAudioBuffer) {
      return;
   }

   m_pAudioBuffer->Play(volume, looping);
}

void SoundProcess::Stop()
{
   if (m_pAudioBuffer) {
      m_pAudioBuffer->Stop();
   }
}

float SoundProcess::GetProgress()
{
   if (m_pAudioBuffer) {
      return m_pAudioBuffer->GetProgress();
   }

   return 0.0f;
}

} // namespace BIEngine
