#pragma once

namespace BIEngine
{

    class IAudioSound
    {
    public:
        virtual ~IAudioSound() = default;

        virtual bool Pause() = 0;
        virtual bool Resume() = 0;
        virtual bool Stop() = 0;

        virtual bool TogglePause() = 0;
        virtual bool IsPlaying() = 0;
        virtual bool IsLooping() const = 0;
        virtual void SetVolume(float volume) = 0;
        virtual float GetVolume() const = 0;
        virtual void SetFrequency(float frequency) = 0;
        virtual float GetFrequency() const = 0;
        virtual void SetPosition(unsigned long newPosition) = 0;
        virtual float GetProgress() = 0;
    };


}