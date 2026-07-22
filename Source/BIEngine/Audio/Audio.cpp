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

    virtual IAudioBuffer* InitAudioBuffer(SharedPtr<ResHandle> pHandle, LoadType loadType) override;
    virtual void ReleaseAudioBuffer(IAudioBuffer* pAudioBuffer) override;

    virtual void Shutdown() override;
    virtual bool Initialize() override;

    virtual void OnUpdate() override;

protected:
    FMOD::System* m_pFMODSystem = nullptr;
};

class fmodAudioBuffer : public AudioBuffer {
public:
    fmodAudioBuffer(FMOD::System* pSoundEngine, SharedPtr<ResHandle> pResource, IAudioManager::LoadType loadType);
    virtual bool OnRestore() override;

    virtual bool Play(int volume, bool looping) override;
    virtual bool Pause() override;
    virtual bool Stop() override;
    virtual bool Resume() override;

    virtual bool TogglePause() override;
    virtual bool IsPlaying() override;
    virtual void SetVolume(int volume) override;
    virtual void SetPosition(unsigned long newPosition) override;

    virtual float GetProgress() override;

private:
    FMOD::System* m_pFMODSystem = nullptr;
    FMOD::Sound* m_pFMODSound = nullptr;
    FMOD::Channel* m_pChannel = nullptr;
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

IAudioBuffer* fmodAudioManager::InitAudioBuffer(SharedPtr<ResHandle> pResHandle, LoadType loadType)
{
    if (!m_pFMODSystem)
    {
        return nullptr;
    }

    IAudioBuffer* pAudioBuffer = new fmodAudioBuffer(m_pFMODSystem, pResHandle, loadType);
    m_allSamples.PushFront(pAudioBuffer);

    return pAudioBuffer;
}

void fmodAudioManager::ReleaseAudioBuffer(IAudioBuffer* pSampleHandle)
{
    pSampleHandle->Stop();
    delete pSampleHandle;
    m_allSamples.Remove(pSampleHandle);
}

fmodAudioBuffer::fmodAudioBuffer(FMOD::System* pSoundEngine, SharedPtr<ResHandle> pResource, IAudioManager::LoadType loadType)
    : AudioBuffer(pResource), m_pFMODSystem(pSoundEngine)
{
    FMOD_CREATESOUNDEXINFO info;
    std::memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));
    info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
    info.length = pResource->Size();

    constexpr FMOD_MODE mode = FMOD_DEFAULT | FMOD_OPENMEMORY | FMOD_LOOP_NORMAL;

    FMOD_RESULT result = FMOD_RESULT::FMOD_OK;

    switch (loadType)
    {
    case IAudioManager::LoadType::DECOMPRESS_ON_LOAD:
        result = m_pFMODSystem->createSound(
            pResource->Buffer(),
            mode,
            &info, &m_pFMODSound);
        break;
    case IAudioManager::LoadType::STREAMING:
        result = m_pFMODSystem->createStream(
            pResource->Buffer(),
            mode,
            &info, &m_pFMODSound);
        break;
    default:
        Assert(false, "Unrecognized load type for audio buffer");
        return;
    }

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

    FMOD_RESULT result = m_pFMODSystem->playSound(m_pFMODSound, nullptr, false, &m_pChannel);
    if (result != FMOD_OK)
    {
        BIEngine::Logger::WriteErrorLog(FMOD_ErrorString(result));
        return false;
    }

    if (m_isLooping)
    {
        m_pChannel->setLoopCount(-1);
    }
    else
    {
        m_pChannel->setLoopCount(0);
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
    if (m_pChannel)
    {
        m_pChannel->stop();
        m_pChannel = nullptr;
    }
    m_pFMODSound->release();
    m_pFMODSound = nullptr;

    return true;
}

bool fmodAudioBuffer::Pause()
{
    if (!g_pAudio->Active() || !m_pChannel)
    {
        return false;
    }

    m_isPaused = true;
    m_pChannel->setPaused(true);
    m_pChannel->setPosition(0, FMOD_TIMEUNIT_MS);

    return true;
}

bool fmodAudioBuffer::Resume()
{
    m_isPaused = false;
    return Play(GetVolume(), IsLooping());
}

bool fmodAudioBuffer::TogglePause()
{
    if (!g_pAudio->Active())
    {
        return false;
    }

    if (m_isPaused)
    {
        Resume();
    }
    else
    {
        Pause();
    }

    return true;
}

bool fmodAudioBuffer::IsPlaying()
{
    if (!g_pAudio->Active() || !m_pChannel)
    {
        return false;
    }

    bool paused = false;
    m_pChannel->getPaused(&paused);
    return !paused;
}

// Громкость может быть в диапазоне от 0 до 100
void fmodAudioBuffer::SetVolume(int volume)
{
    if (!g_pAudio->Active() || !m_pChannel)
    {
        return;
    }

    Assert(volume >= 0 && volume <= 100, "Volume must be a number between 0 and 100");
    if (volume < 0 || volume > 100) {
        return;
    }

    const float coeff = static_cast<float>(volume) / 100.0f;
    m_pChannel->setVolume(coeff);
}

void fmodAudioBuffer::SetPosition(unsigned long newPosition)
{
    if (!g_pAudio->Active() || !m_pChannel)
        return;

    m_pChannel->setPosition(newPosition, FMOD_TIMEUNIT_MS);
}

bool fmodAudioBuffer::OnRestore()
{
    return true;
}

// Возвращает долю величины уже проигранной части музыки. Возвращается в диапазоне от 0.0 до 1.0
float fmodAudioBuffer::GetProgress()
{
    if (!g_pAudio->Active() || !m_pChannel)
    {
        return 0.0f;
    }

    unsigned int length = 0;
    m_pFMODSound->getLength(&length, FMOD_TIMEUNIT_MS);

    if (length == 0)
    {
        return 0.0f;
    }

    unsigned int progress = 0.0f;
    m_pChannel->getPosition(&progress, FMOD_TIMEUNIT_MS);

    return static_cast<float>(progress) / length;
}

AudioManager* CreateAudioManager()
{
    AudioManager* pAudioManager = new fmodAudioManager();
    return pAudioManager;
}


} // namespace BIEngine
