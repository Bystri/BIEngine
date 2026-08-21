#pragma once

#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class IAudioSound
{
public:
    virtual ~IAudioSound() = default;

    virtual bool Pause() = 0;
    virtual bool Resume() = 0;
    virtual bool Stop() = 0;

    virtual bool TogglePause() = 0;
    virtual bool IsPlaying() = 0;
    virtual bool IsLooping() const = 0;
    virtual void SetVolume(float volume) = 0;
    virtual float GetVolume() const = 0;
    virtual void SetFrequency(float frequency) = 0;
    virtual float GetFrequency() const = 0;
    virtual void SetPosition(unsigned long newPosition) = 0;
    virtual float GetProgress() = 0;
};

class IAudioBuffer {
public:
   virtual ~IAudioBuffer() = default;

   virtual IAudioSound* Play(float volume, bool looping) = 0;
   virtual bool Release() = 0;

   void ReleaseAudio(IAudioSound* pAudio);

   void StopAllSounds();
   void PauseAllSounds();
   void ResumeAllSounds();

protected:
    using AudioSoundList = List<IAudioSound*>;
    AudioSoundList m_allSounds;
};

class IAudioManager {
public:
   virtual ~IAudioManager() = default;

   enum class LoadType
   {
      DECOMPRESS_ON_LOAD,
      STREAMING
   };

   virtual bool Active() = 0;

   virtual IAudioBuffer* InitAudioBuffer(SharedPtr<ResHandle> handle, LoadType loadType) = 0;
   virtual void ReleaseAudioBuffer(IAudioBuffer* audioBuffer) = 0;

   virtual void StopAllSounds() = 0;
   virtual void PauseAllSounds() = 0;
   virtual void ResumeAllSounds() = 0;

   virtual bool Initialize() = 0;
   virtual void Shutdown() = 0;

   virtual void OnUpdate() {}
};

// Главная аудио система, управляющая аудио-буферами
class AudioManager : public IAudioManager {
public:
   AudioManager();

   virtual ~AudioManager() = default;

   virtual void StopAllSounds();
   virtual void PauseAllSounds();
   virtual void ResumeAllSounds();

   virtual void Shutdown();
   static bool HasSoundCard();

   bool IsPaused() { return m_allPaused; }

protected:
   using AudioBufferList = List<IAudioBuffer*>;

   AudioBufferList m_allBuffers;
   bool m_allPaused = false;
   bool m_initialized;
};

extern AudioManager* g_pAudio;

extern AudioManager* CreateAudioManager();

} // namespace BIEngine
