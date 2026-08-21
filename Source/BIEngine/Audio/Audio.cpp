#include "Audio.h"

#include <fmod.hpp>
#include <fmod_errors.h>

#include "SoundLoader.h"
#include "../Utilities/Logger.h"
#include "../StdLib/Algorithm.h"

namespace BIEngine {

AudioManager* g_pAudio = nullptr;

AudioManager::AudioManager()
   : m_initialized(false)
{
}

void AudioManager::Shutdown()
{
   while (!m_allBuffers.Empty()) {
      ReleaseAudioBuffer(m_allBuffers.Front());
   }

   m_allPaused = false;
}

bool AudioManager::HasSoundCard()
{
   return (g_pAudio && g_pAudio->Active());
}

void AudioManager::PauseAllSounds()
{
    for (auto& audioBuffer : m_allBuffers) {
        audioBuffer->PauseAllSounds();
    }

    m_allPaused = true;
}

void AudioManager::ResumeAllSounds()
{
    for (auto& audioBuffer : m_allBuffers) {
        audioBuffer->ResumeAllSounds();
    }

    m_allPaused = false;
}

void AudioManager::StopAllSounds()
{
    for (auto& audioBuffer : m_allBuffers) {
        audioBuffer->StopAllSounds();
    }

    m_allPaused = false;
}

void IAudioBuffer::ReleaseAudio(IAudioSound* pAudio)
{
    if (!pAudio) {
        return;
    }

    auto itr = Find(m_allSounds.Begin(), m_allSounds.End(), pAudio);

    Assert(itr != m_allSounds.End(), "Used incorrect AudioBuffer to release audio");

    pAudio->Stop();
    m_allSounds.Erase(itr);
    delete pAudio;
}

void IAudioBuffer::PauseAllSounds()
{
    for (auto& sound : m_allSounds) {
        sound->Pause();
    }
}

void IAudioBuffer::ResumeAllSounds()
{
    for (auto& sound : m_allSounds) {
        sound->Resume();
    }
}

void IAudioBuffer::StopAllSounds()
{
    for (auto& sound : m_allSounds) {
        sound->Stop();
    }

    m_allSounds.Clear();
}

class fmodAudioManager : public AudioManager {
public:
    virtual ~fmodAudioManager() { Shutdown(); }

    virtual bool Active() { return m_pFMODSystem != nullptr; }

    virtual IAudioBuffer* InitAudioBuffer(SharedPtr<ResHandle> pHandle, LoadType loadType) override;
    virtual void ReleaseAudioBuffer(IAudioBuffer* pAudioBuffer) override;

    virtual void Shutdown() override;
    virtual bool Initialize() override;

    virtual void OnUpdate() override;

protected:
    FMOD::System* m_pFMODSystem = nullptr;
};

class fmodAudioSound : public IAudioSound
{
public:
    fmodAudioSound(FMOD::Sound* m_pFMODSound, FMOD::Channel* m_pChannel);

    virtual bool Pause() override;
    virtual bool Resume() override;
    virtual bool Stop() override;

    virtual bool TogglePause() override;
    virtual bool IsPlaying() override;
    virtual bool IsLooping() const override;
    virtual void SetVolume(float volume) override;
    virtual float GetVolume() const override;

    virtual void SetPosition(unsigned long newPosition) override;

    virtual void SetFrequency(float frequency) override;
    virtual float GetFrequency() const override;

    virtual float GetProgress() override;

private:
    FMOD::Sound* const m_pFMODSound = nullptr;
    FMOD::Channel* m_pChannel = nullptr;
};

class fmodAudioBuffer : public IAudioBuffer {
public:
    fmodAudioBuffer(FMOD::System* pSoundEngine, SharedPtr<ResHandle> pResource, IAudioManager::LoadType loadType);

    bool IsValid() const { return m_pFMODSound != nullptr; }

    virtual IAudioSound* Play(float volume, bool looping) override;
    virtual bool Release() override;

private:
    FMOD::System* m_pFMODSystem = nullptr;
    FMOD::Sound* m_pFMODSound = nullptr;
    SharedPtr<ResHandle> m_pResource;
};

bool fmodAudioManager::Initialize()
{
    if (m_initialized)
    {
        return true;
    }

    Shutdown();

    FMOD_RESULT result = FMOD::System_Create(&m_pFMODSystem);
    if (result != FMOD_OK || !m_pFMODSystem)
    {
        Logger::WriteErrorLog(FMOD_ErrorString(result));
        return false;
    }

    result = m_pFMODSystem->init(100, FMOD_INIT_NORMAL, 0);
    if (result != FMOD_OK)
    {
        Logger::WriteErrorLog(FMOD_ErrorString(result));
        m_pFMODSystem->release();
        m_pFMODSystem = nullptr;
        return false;
    }

    m_initialized = true;

    return true;
}

void fmodAudioManager::OnUpdate()
{
    if (m_pFMODSystem) {
        m_pFMODSystem->update();
    }
}

void fmodAudioManager::Shutdown()
{
    if (m_pFMODSystem) {
        AudioManager::Shutdown();
        m_pFMODSystem->release();
        m_pFMODSystem = nullptr;
    }

    m_initialized = false;
}

IAudioBuffer* fmodAudioManager::InitAudioBuffer(SharedPtr<ResHandle> pResHandle, LoadType loadType)
{
    if (!m_pFMODSystem || !pResHandle)
    {
        return nullptr;
    }

    fmodAudioBuffer* pAudioBuffer = new fmodAudioBuffer(m_pFMODSystem, pResHandle, loadType);
    if (!pAudioBuffer->IsValid()) {
        delete pAudioBuffer;
        return nullptr;
    }

    m_allBuffers.PushFront(pAudioBuffer);

    return pAudioBuffer;
}

void fmodAudioManager::ReleaseAudioBuffer(IAudioBuffer* pSampleHandle)
{
    if (!pSampleHandle) {
        return;
    }

    pSampleHandle->StopAllSounds();
    pSampleHandle->Release();
    m_allBuffers.Remove(pSampleHandle);
    delete pSampleHandle;
}

fmodAudioBuffer::fmodAudioBuffer(FMOD::System* pSoundEngine, SharedPtr<ResHandle> pResource, IAudioManager::LoadType loadType)
    : IAudioBuffer(), m_pFMODSystem(pSoundEngine), m_pResource(pResource)
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

IAudioSound* fmodAudioBuffer::Play(float volume, bool looping)
{
    Assert(volume >= 0.0f && volume <= 1.0f, "Volume must be a number between 0.0 and 1.0");
    if (volume < 0.0f || volume > 1.0f) {
        return nullptr;
    }

    if (!AudioManager::HasSoundCard())
    {
        return nullptr;
    }

    if (!m_pFMODSystem || !m_pFMODSound)
    {
        return nullptr;
    }

    FMOD::Channel* pChannel = nullptr;
    FMOD_RESULT result = m_pFMODSystem->playSound(m_pFMODSound, nullptr, true, &pChannel);
    if (result != FMOD_OK)
    {
        BIEngine::Logger::WriteErrorLog(FMOD_ErrorString(result));
        return nullptr;
    }

    result = pChannel->setLoopCount(looping ? -1 : 0);
    if (result != FMOD_OK) {
        Logger::WriteErrorLog(FMOD_ErrorString(result));
        pChannel->stop();
        return nullptr;
    }

    IAudioSound* pSound = new fmodAudioSound(m_pFMODSound, pChannel);

    result = pChannel->setVolume(volume);
    if (result == FMOD_OK) {
        result = pChannel->setPaused(false);
    }
    if (result != FMOD_OK) {
        Logger::WriteErrorLog(FMOD_ErrorString(result));
        pChannel->stop();
        return nullptr;
    }

    m_allSounds.PushBack(pSound);

    return pSound;
}

fmodAudioSound::fmodAudioSound(FMOD::Sound* m_pFMODSound, FMOD::Channel* m_pChannel)
    : m_pFMODSound(m_pFMODSound)
    , m_pChannel(m_pChannel)
{

}

bool fmodAudioBuffer::Release()
{
    if (!m_pFMODSound)
    {
        return false;
    }

    const FMOD_RESULT result = m_pFMODSound->release();
    m_pFMODSound = nullptr;
    m_pResource.Reset();

    return result == FMOD_OK;
}

bool fmodAudioSound::Pause()
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return false;
    }

    return m_pChannel->setPaused(true) == FMOD_OK;
}

bool fmodAudioSound::Resume()
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return false;
    }

    return m_pChannel->setPaused(false) == FMOD_OK;
}

bool fmodAudioSound::Stop()
{
    if (!m_pChannel)
    {
        return false;
    }

    if (!AudioManager::HasSoundCard()) {
        m_pChannel = nullptr;
        return false;
    }

    const FMOD_RESULT result = m_pChannel->stop();
    m_pChannel = nullptr;

    return result == FMOD_OK;
}

bool fmodAudioSound::TogglePause()
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return false;
    }

    bool paused = false;
    if (m_pChannel->getPaused(&paused) != FMOD_OK) {
        return false;
    }

    return m_pChannel->setPaused(!paused) == FMOD_OK;
}

bool fmodAudioSound::IsPlaying()
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return false;
    }

    bool isPlaying = false;
    if (m_pChannel->isPlaying(&isPlaying) != FMOD_OK || !isPlaying) {
        m_pChannel = nullptr;
        return false;
    }

    return true;
}

bool fmodAudioSound::IsLooping() const
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return false;
    }

    int loopCount = -1;
    return m_pChannel->getLoopCount(&loopCount) == FMOD_OK && loopCount == -1;
}

void fmodAudioSound::SetVolume(float volume)
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return;
    }

    Assert(volume >= 0 && volume <= 1.0f, "Volume must be a number between 0.0 and 1.0");
    if (volume < 0 || volume > 1.0f) {
        return;
    }

    m_pChannel->setVolume(volume);
}

float fmodAudioSound::GetVolume() const
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return 0.0f;
    }

    float volume = 0.0f;
    if (m_pChannel->getVolume(&volume) != FMOD_OK) {
        return 0.0f;
    }

    return volume;
}

void fmodAudioSound::SetFrequency(float frequency)
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return;
    }

    m_pChannel->setFrequency(frequency);
}

float fmodAudioSound::GetFrequency() const
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return 0.0f;
    }

    float freq = 0.0f;
    if (m_pChannel->getFrequency(&freq) != FMOD_OK)
    {
        return 0.0f;
    }

    return freq;
}

void fmodAudioSound::SetPosition(unsigned long newPosition)
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
        return;

    m_pChannel->setPosition(newPosition, FMOD_TIMEUNIT_MS);
}

float fmodAudioSound::GetProgress()
{
    if (!AudioManager::HasSoundCard() || !m_pChannel)
    {
        return 0.0f;
    }

    unsigned int length = 0;
    if (m_pFMODSound->getLength(&length, FMOD_TIMEUNIT_MS) != FMOD_OK || length == 0)
    {
        return 0.0f;
    }

    unsigned int progress = 0;
    if (m_pChannel->getPosition(&progress, FMOD_TIMEUNIT_MS) != FMOD_OK) {
        return 0.0f;
    }

    return static_cast<float>(progress) / length;
}

AudioManager* CreateAudioManager()
{
    AudioManager* pAudioManager = new fmodAudioManager();
    return pAudioManager;
}


} // namespace BIEngine
