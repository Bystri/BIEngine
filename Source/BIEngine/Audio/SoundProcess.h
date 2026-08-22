#pragma once

#include "Audio.h"
#include "SoundLoader.h"
#include "../Utilities/GameTimer.h"
#include "../ProcessManager/Process.h"

namespace BIEngine {

class SoundProcess : public Process {
public:
   SoundProcess(SharedPtr<ResHandle> pSoundResource, IAudioBuffer::LoadType loadType, float volume = 1.0f, bool looping = false);
   virtual ~SoundProcess();

   void Play(const float volume, const bool looping);
   void Stop();

   // Задает громкость звука. Громкость может быть от 0.0 до 1.0
   void SetVolume(float volume);
   float GetVolume();

   bool IsSoundValid() const { return m_pHandle != nullptr; }

   bool IsPlaying() const;

   bool IsLooping() const { return m_pAudioSound && m_pAudioSound->IsLooping(); }

   float GetProgress() const;
   void PauseSound();

protected:
   // Не позволяет использовать конструктор по-умолчанию из вне
   SoundProcess();

   virtual void OnInit();
   virtual void OnUpdate(float dt) override;

protected:
   SharedPtr<ResHandle> m_pHandle;
   IAudioSound* m_pAudioSound;
   IAudioBuffer* m_pAudioBuffer = nullptr;

   float m_volume;
   bool m_isLooping;
   const IAudioBuffer::LoadType m_loadType;
};

} // namespace BIEngine
