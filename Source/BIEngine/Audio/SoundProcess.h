#pragma once

#include "Audio.h"
#include "SoundLoader.h"
#include "../Utilities/GameTimer.h"
#include "../ProcessManager/Process.h"

namespace BIEngine {

class SoundProcess : public Process {
public:
   SoundProcess(SharedPtr<ResHandle> pSoundResource, IAudioManager::LoadType loadType, float volume = 1.0f, bool looping = false);
   virtual ~SoundProcess();

   void Play(const float volume, const bool looping);
   void Stop();

   // Задает громкость звука. Громкость может быть от 0 до 100
   void SetVolume(float volume);
   float GetVolume();
   int GetLengthMilli() const;

   bool IsSoundValid() const { return m_pHandle != NULL; }

   bool IsPlaying() const;

   bool IsLooping() const { return m_pAudioBuffer && m_pAudioBuffer->IsLooping(); }

   float GetProgress() const;
   void PauseSound();

protected:
   // Не позволяет использовать конструктор по-умолчанию из вне
   SoundProcess();

   virtual void OnInit();
   virtual void OnUpdate(float dt) override;

protected:
   SharedPtr<ResHandle> m_pHandle;
   SharedPtr<IAudioBuffer> m_pAudioBuffer;

   float m_volume;
   bool m_isLooping;
   const IAudioManager::LoadType m_loadType;
};

} // namespace BIEngine
