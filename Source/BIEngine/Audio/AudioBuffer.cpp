#include "AudioBuffer.h"

#include "../StdLib/Algorithm.h"
#include "../StdLib/Assert.h"

namespace BIEngine
{

    void AudioBuffer::ReleaseAudio(IAudioSound* pAudio)
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

    bool AudioBuffer::Release()
    {
        while (!m_allSounds.Empty()) {
            ReleaseAudio(m_allSounds.Front());
        }

        return true;
    }

    void AudioBuffer::PauseAllSounds()
    {
        for (auto& sound : m_allSounds) {
            sound->Pause();
        }
    }

    void AudioBuffer::ResumeAllSounds()
    {
        for (auto& sound : m_allSounds) {
            sound->Resume();
        }
    }

    void AudioBuffer::StopAllSounds()
    {
        for (auto& sound : m_allSounds) {
            sound->Stop();
        }

        m_allSounds.Clear();
    }

}