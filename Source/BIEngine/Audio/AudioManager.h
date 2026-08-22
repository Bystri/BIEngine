#pragma once

#include "AudioBuffer.h"
#include "../StdLib/SharedPtr.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine
{

    class IAudioManager {
    public:
        virtual ~IAudioManager() = default;

        virtual bool Active() = 0;

        virtual IAudioBuffer* InitAudioBuffer(SharedPtr<ResHandle> handle, IAudioBuffer::LoadType loadType) = 0;
        virtual void ReleaseAudioBuffer(IAudioBuffer* audioBuffer) = 0;

        virtual void StopAllSounds() = 0;
        virtual void PauseAllSounds() = 0;
        virtual void ResumeAllSounds() = 0;

        virtual bool Initialize() = 0;
        virtual void Shutdown() = 0;

        virtual void OnUpdate() = 0;
    };

    class AudioManager : public IAudioManager {
    public:
        virtual ~AudioManager() = default;

        virtual void StopAllSounds();
        virtual void PauseAllSounds();
        virtual void ResumeAllSounds();

        virtual void Shutdown();

    protected:
        using AudioBufferList = List<IAudioBuffer*>;

        AudioBufferList m_allBuffers;
        bool m_allPaused = false;
        bool m_initialized = false;
    };

}