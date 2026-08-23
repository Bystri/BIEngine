#pragma once

#include "AudioBuffer.h"
#include "AudioGroupManager.h"
#include "../StdLib/SharedPtr.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine
{

    class IAudioManager {
    public:
        virtual ~IAudioManager() = default;

        virtual bool Active() = 0;

        virtual IAudioBuffer* InitAudioBuffer(char* rawBuffer, unsigned int rawSize, IAudioBuffer::LoadType loadType) = 0;
        virtual void ReleaseAudioBuffer(IAudioBuffer* audioBuffer) = 0;

        virtual void StopAllSounds() = 0;
        virtual void PauseAllSounds() = 0;
        virtual void ResumeAllSounds() = 0;

        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual AudioGroupManager* GetAudioGroupManager() = 0;

        virtual void OnUpdate() = 0;
    };

    class AudioManager : public IAudioManager {
    public:
        virtual void StopAllSounds() override;
        virtual void PauseAllSounds() override;
        virtual void ResumeAllSounds() override;

        virtual void Shutdown() override;

        virtual AudioGroupManager* GetAudioGroupManager() override;

    protected:
        using AudioBufferList = List<IAudioBuffer*>;

        AudioBufferList m_allBuffers;

        AudioGroupManager* m_pAudioGroupManager;

        bool m_allPaused = false;
        bool m_initialized = false;
    };

}