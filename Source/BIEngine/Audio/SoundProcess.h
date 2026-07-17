#pragma once

#include "Audio.h"
#include "SoundLoader.h"
#include "../Utilities/GameTimer.h"
#include "../ProcessManager/Process.h"

namespace BIEngine {

class SoundProcess : public Process {
public:
   SoundProcess(SharedPtr<ResHandle> pSoundResource, int volume = 100, bool looping = false);
   virtual ~SoundProcess();

   void Play(const int volume, const bool looping);
   void Stop();

   // Задает громкость звука. Громкость может быть от 0 до 100
   void SetVolume(int volume);
   int GetVolume();
   int GetLengthMilli();

   bool IsSoundValid() { return m_pHandle != NULL; }

   bool IsPlaying();

   bool IsLooping() { return m_pAudioBuffer && m_pAudioBuffer->IsLooping(); }

   float GetProgress();
   void PauseSound();

protected:
   // Не позволяет использовать конструктор по-умолчанию из вне
   SoundProcess();

   virtual void OnInit();
   virtual void OnUpdate(float dt) override;

protected:
   SharedPtr<ResHandle> m_pHandle;
   SharedPtr<IAudioBuffer> m_pAudioBuffer;

   int m_volume;
   bool m_isLooping;
};

} // namespace BIEngine
