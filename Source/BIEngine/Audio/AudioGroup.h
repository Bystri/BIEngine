#pragma once

#include "AudioSound.h"

namespace BIEngine
{

    class IAudioGroup
    {
    public:
        IAudioGroup() = default;
        virtual ~IAudioGroup() = default;

        IAudioGroup(const IAudioGroup&) = delete;
        IAudioGroup& operator=(const IAudioGroup&) = delete;

        virtual void ApplyToSound(IAudioSound* pAudioSound) = 0;

        virtual void SetVolume(float volume) = 0;
        virtual float GetVolume() const = 0;
    };

}