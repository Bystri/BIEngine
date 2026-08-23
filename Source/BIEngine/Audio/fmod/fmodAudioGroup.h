#pragma once

#include <fmod.hpp>

#include "../AudioGroup.h"

namespace BIEngine
{

    class fmodAudioGroup : public IAudioGroup
    {
    public:
        fmodAudioGroup(FMOD::ChannelGroup* pChannel);

        virtual void ApplyToSound(IAudioSound* pAudioSound) override;

        virtual void SetVolume(float volume) override;
        virtual float GetVolume() const override;
    
    private:
        FMOD::ChannelGroup* const m_pChannelGroup;
    };

}