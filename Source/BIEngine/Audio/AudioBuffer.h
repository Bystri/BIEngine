#pragma once

#include "AudioSound.h"
#include "../StdLib/List.h"

namespace BIEngine
{

    class IAudioBuffer {
    public:
        enum class LoadType
        {
            DECOMPRESS_ON_LOAD,
            STREAMING
        };

        virtual ~IAudioBuffer() = default;

        virtual bool Release() = 0;

        virtual IAudioSound* Play(float volume, bool looping) = 0;
        virtual void ReleaseAudio(IAudioSound* pAudio) = 0;

        virtual void StopAllSounds() = 0;
        virtual void PauseAllSounds() = 0;
        virtual void ResumeAllSounds() = 0;
    };

    class AudioBuffer : public IAudioBuffer {
    public:
        virtual void ReleaseAudio(IAudioSound* pAudio) override;

        virtual bool Release() override;

        virtual void StopAllSounds() override;
        virtual void PauseAllSounds() override;
        virtual void ResumeAllSounds() override;

    protected:
        using AudioSoundList = List<IAudioSound*>;
        AudioSoundList m_allSounds;
    };
}