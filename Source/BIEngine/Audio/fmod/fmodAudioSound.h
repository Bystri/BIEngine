#pragma once

#include "../AudioSound.h"

#include <fmod.hpp>
#include <fmod_errors.h>

namespace BIEngine
{

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

        FMOD::Channel* GetChannel() const { return m_pChannel; }

    private:
        FMOD::Sound* const m_pFMODSound = nullptr;
        FMOD::Channel* m_pChannel = nullptr;
    };

}