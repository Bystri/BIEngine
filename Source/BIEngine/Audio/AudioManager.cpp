#include "AudioManager.h"

namespace BIEngine
{

    AudioManager* g_pAudio = nullptr;

    void AudioManager::Shutdown()
    {
        while (!m_allBuffers.Empty()) {
            ReleaseAudioBuffer(m_allBuffers.Front());
        }

        m_allPaused = false;
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

    AudioGroupManager* AudioManager::GetAudioGroupManager()
    {
        return m_pAudioGroupManager;
    }
}