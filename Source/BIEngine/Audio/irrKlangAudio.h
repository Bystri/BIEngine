#pragma once

#include <irrKlang.h>

#include "Audio.h"

namespace BIEngine {

class irrKlangAudioBuffer : public AudioBuffer {
public:
   irrKlangAudioBuffer(irrklang::ISoundEngine* pSoundEngine, SharedPtr<ResHandle> pResource);
   virtual bool OnRestore();

   virtual bool Play(int volume, bool looping);
   virtual bool Pause();
   virtual bool Stop();
   virtual bool Resume();

   virtual bool TogglePause();
   virtual bool IsPlaying();
   virtual void SetVolume(int volume);
   virtual void SetPosition(unsigned long newPosition);

   virtual float GetProgress();

private:
   irrklang::ISound* m_pSound;
   irrklang::ISoundSource* m_pSoundSource;
   irrklang::ISoundEngine* m_pSoundEngine;
};

class irrKlangAudio : public AudioManager {
public:
   irrKlangAudio()
      : m_pSoundEngine(nullptr) {}

   virtual bool Active() { return m_pSoundEngine != nullptr; }

   virtual IAudioBuffer* InitAudioBuffer(SharedPtr<ResHandle> pHandle);
   virtual void ReleaseAudioBuffer(IAudioBuffer* pAudioBuffer);

   virtual void Shutdown();
   virtual bool Initialize();

protected:
   irrklang::ISoundEngine* m_pSoundEngine;
};

} // namespace BIEngine
