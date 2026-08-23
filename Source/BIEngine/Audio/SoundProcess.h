#pragma once

#include "Audio.h"
#include "SoundLoader.h"
#include "../Utilities/GameTimer.h"
#include "../ProcessManager/Process.h"

namespace BIEngine {

class SoundProcess : public Process {
public:
   SoundProcess(SharedPtr<ResHandle> pSoundBufferResource, IAudioSound* pSound);
   virtual ~SoundProcess();

   void TogglePause();

   // Задает громкость звука. Громкость может быть от 0.0 до 1.0
   void SetVolume(float volume);
   float GetVolume();

   bool IsSoundValid() const { return m_pHandle != nullptr; }
   bool IsPlaying() const;
   bool IsLooping() const { return m_pAudioSound && m_pAudioSound->IsLooping(); }
   float GetProgress() const;

protected:

   virtual void OnInit();
   virtual void OnUpdate(float dt) override;

private:
    void Stop();

protected:
   SharedPtr<ResHandle> m_pHandle;
   IAudioSound* const m_pAudioSound = nullptr;
};

} // namespace BIEngine
