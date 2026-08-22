#include "fmodAudioBuffer.h"

#include "fmodAudioSound.h"
#include "../../Utilities/Logger.h"

namespace BIEngine
{

    fmodAudioBuffer::fmodAudioBuffer(FMOD::System* pSoundEngine, SharedPtr<ResHandle> pResource, IAudioBuffer::LoadType loadType)
        : AudioBuffer(), m_pFMODSystem(pSoundEngine), m_pResource(pResource)
    {
        FMOD_CREATESOUNDEXINFO info;
        std::memset(&info, 0, sizeof(FMOD_CREATESOUNDEXINFO));
        info.cbsize = sizeof(FMOD_CREATESOUNDEXINFO);
        info.length = pResource->Size();

        constexpr FMOD_MODE mode = FMOD_DEFAULT | FMOD_OPENMEMORY | FMOD_LOOP_NORMAL;

        FMOD_RESULT result = FMOD_RESULT::FMOD_OK;

        switch (loadType)
        {
        case IAudioBuffer::LoadType::DECOMPRESS_ON_LOAD:
            result = m_pFMODSystem->createSound(
                pResource->Buffer(),
                mode,
                &info, &m_pFMODSound);
            break;
        case IAudioBuffer::LoadType::STREAMING:
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
}