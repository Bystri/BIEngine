#include "Audio.h"

#include <fmod.hpp>
#include <fmod_errors.h>

#include "SoundLoader.h"
#include "../Utilities/Logger.h"

namespace BIEngine {

AudioManager* g_pAudio = nullptr;
char* gSoundExtentions[] = {".mp3", ".wav", ".midi", ".ogg"};

AudioManager::AudioManager()
   : m_initialized(false),
     m_allPaused(false)
{
}

void AudioManager::Shutdown()
{
   AudioBufferList::Iterator itr = m_allSamples.Begin();

   while (itr != m_allSamples.End()) {
      (*itr)->Stop();
      ReleaseAudioBuffer(*itr);
      ++itr;
      m_allSamples.PopFront();
   }
}

void AudioManager::PauseAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Pause();
   }

   m_allPaused = true;
}

void AudioManager::ResumeAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Resume();
   }

   m_allPaused = false;
}

void AudioManager::StopAllSounds()
{
   for (auto& audioBuffer : m_allSamples) {
      audioBuffer->Stop();
   }

   m_allPaused = false;
}

bool AudioManager::HasSoundCard()
{
   return (g_pAudio && g_pAudio->Active());
}

class fmodAudioManager : public AudioManager {
public:
    virtual bool Active() { return m_pFMODSystem != nullptr; }

    virtual IAudioBuffer* InitAudioBuffer(SharedPtr<ResHandle> pHandle);
    virtual void ReleaseAudioBuffer(IAudioBuffer* pAudioBuffer);

    virtual void Shutdown();
    virtual bool Initialize();

    virtual void OnUpdate() override;

protected:
    FMOD::System* m_pFMODSystem = nullptr;
};

class fmodAudioBuffer : public AudioBuffer {
public:
    fmodAudioBuffer(FMOD::System* pSoundEngine, SharedPtr<ResHandle> pResource);
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
    FMOD::System* m_pFMODSystem = nullptr;
    FMOD::Sound* m_pFMODSound = nullptr;
};

bool fmodAudioManager::Initialize()
{
    if (m_initialized)
    {
        return true;
    }

    Shutdown();

    FMOD::System_Create(&m_pFMODSystem);

    if (!m_pFMODSystem)
    {
        return false;
    }

    FMOD_RESULT result = m_pFMODSystem->init(100, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        Logger::WriteErrorLog(FMOD_ErrorString(result));
        return false;
    }

    m_initialized = true;

    return true;
}

void fmodAudioManager::OnUpdate()
{
    m_pFMODSystem->update();
}

void fmodAudioManager::Shutdown()
{
    if (m_initialized) {
        AudioManager::Shutdown();
        if (m_pFMODSystem) {
            m_pFMODSystem->release();
            m_pFMODSystem = nullptr;
        }
        m_initialized = false;
    }
}

IAudioBuffer* fmodAudioManager::InitAudioBuffer(SharedPtr<ResHandle> pResHandle)
{
    if (!m_pFMODSystem)
    {
        return nullptr;
    }

    IAudioBuffer* pAudioBuffer = new fmodAudioBuffer(m_pFMODSystem, pResHandle);
    m_allSamples.PushFront(pAudioBuffer);

    return pAudioBuffer;
}

void fmodAudioManager::ReleaseAudioBuffer(IAudioBuffer* pSampleHandle)
{
    pSampleHandle->Stop();
    delete pSampleHandle;
    m_allSamples.Remove(pSampleHandle);
}

fmodAudioBuffer::fmodAudioBuffer(FMOD::System* pSoundEngine, SharedPtr<ResHandle> pResource)
    : AudioBuffer(pResource), m_pFMODSystem(pSoundEngine)
{
    FMOD_CREATESOUNDEXINFO info;
    std::memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    info.length = pResource->Size();

    FMOD_RESULT result = m_pFMODSystem->createSound(
        pResource->Buffer(), 
        FMOD_DEFAULT | FMOD_OPENMEMORY, 
        &info, &m_pFMODSound);
    if (result != FMOD_OK)
    {
        BIEngine::Logger::WriteErrorLog(FMOD_ErrorString(result));
        return;
    }
}

bool fmodAudioBuffer::Play(int volume, bool looping)
{
    if (!g_pAudio->Active())
    {
        return false;
    }

    m_volume = volume;
    m_isLooping = looping;

    if (!m_pFMODSystem || !m_pFMODSound)
    {
        return false;
    }

    SetVolume(volume);

    FMOD_RESULT result = m_pFMODSystem->playSound(m_pFMODSound, nullptr, false, nullptr);
    if (result != FMOD_OK)
    {
        BIEngine::Logger::WriteErrorLog(FMOD_ErrorString(result));
        return false;
    }

    return true;
}

bool fmodAudioBuffer::Stop()
{
    if (!g_pAudio->Active() || !m_pFMODSound)
    {
        return false;
    }

    m_isPaused = true;
    m_pFMODSound->release();
    m_pFMODSound = nullptr;

    return true;
}

bool fmodAudioBuffer::Pause()
{
    /*
    if (!g_pAudio->Active() || !m_pSound)
        return false;

    m_isPaused = true;
    m_pSound->setIsPaused();
    m_pSound->setPlayPosition(0);
    return true;
    */

    return false;
}

bool fmodAudioBuffer::Resume()
{
    m_isPaused = false;
    return Play(GetVolume(), IsLooping());
}

bool fmodAudioBuffer::TogglePause()
{
    /*
    if (!g_pAudio->Active())
        return false;

    if (m_isPaused)
        Resume();
    else
        Pause();

    return true;
    */

    return false;
}

bool fmodAudioBuffer::IsPlaying()
{
    /*
    if (!g_pAudio->Active() || !m_pSound)
        return false;

    bool b = m_pSound->isFinished();
    return !m_pSound->isFinished() && !m_pSound->getIsPaused();
    */

    return true;
}

// Громкость может быть в диапазоне от 0 до 100
void fmodAudioBuffer::SetVolume(int volume)
{
    /*
    if (!g_pAudio->Active() || !m_pSound)
        return;

    Assert(volume >= 0 && volume <= 100, "Volume must be a number between 0 and 100");
    if (volume < 0 || volume > 100) {
        return;
    }

    float coeff = (float)volume / 100.0f;
    m_pSound->setVolume(coeff);
    */
}

void fmodAudioBuffer::SetPosition(unsigned long newPosition)
{
    /*
    if (!g_pAudio->Active() || !m_pSound)
        return;

    m_pSound->setPlayPosition(newPosition);
    */
}

bool fmodAudioBuffer::OnRestore()
{
    return true;
}

// Возвращает долю величины уже проигранной части музыки. Возвращается в диапазоне от 0.0 до 1.0
float fmodAudioBuffer::GetProgress()
{
    /*
    if (!g_pAudio->Active() || !m_pSound)
        return 0.f;

    float progress = m_pSound->getPlayPosition();
    float length = m_pSound->getPlayLength();

    return progress / length;
    */

    return 0.0f;
}

AudioManager* CreateAudioManager()
{
    AudioManager* pAudioManager = new fmodAudioManager();
    return pAudioManager;
}


} // namespace BIEngine
