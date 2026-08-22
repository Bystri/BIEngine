#include "fmodAudioSound.h"

#include "../../Utilities/Logger.h"

namespace BIEngine
{

    fmodAudioSound::fmodAudioSound(FMOD::Sound* m_pFMODSound, FMOD::Channel* m_pChannel)
        : m_pFMODSound(m_pFMODSound)
        , m_pChannel(m_pChannel)
    {

    }

    bool fmodAudioSound::Pause()
    {
        if (!m_pChannel)
        {
            return false;
        }

        return m_pChannel->setPaused(true) == FMOD_OK;
    }

    bool fmodAudioSound::Resume()
    {
        if (!m_pChannel)
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

        const FMOD_RESULT result = m_pChannel->stop();
        m_pChannel = nullptr;

        return result == FMOD_OK;
    }

    bool fmodAudioSound::TogglePause()
    {
        if (!m_pChannel)
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
        if (!m_pChannel)
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
        if (!m_pChannel)
        {
            return false;
        }

        int loopCount = -1;
        return m_pChannel->getLoopCount(&loopCount) == FMOD_OK && loopCount == -1;
    }

    void fmodAudioSound::SetVolume(float volume)
    {
        if (!m_pChannel)
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
        if (!m_pChannel)
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
        if (!m_pChannel)
        {
            return;
        }

        m_pChannel->setFrequency(frequency);
    }

    float fmodAudioSound::GetFrequency() const
    {
        if (!m_pChannel)
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
        if (!m_pChannel)
        {
            return;
        }

        m_pChannel->setPosition(newPosition, FMOD_TIMEUNIT_MS);
    }

    float fmodAudioSound::GetProgress()
    {
        if (!m_pChannel)
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


}