#pragma once

#include "../ProcessManager/Process.h"
#include "Audio.h"
#include "SoundLoader.h"

namespace BIEngine
{

    class SoundProcess : public Process
    {
    public:
        SoundProcess(std::shared_ptr<ResHandle> pSoundResource, int volume = 100, bool looping = false);
        virtual ~SoundProcess();

        void Play(const int volume, const bool looping);
        void Stop();

        //Задает громкость звука. Громкость может быть от 0 до 100
        void SetVolume(int volume);
        int GetVolume();
        int GetLengthMilli();
        bool IsSoundValid() { return m_pHandle != NULL; }
        bool IsPlaying();
        bool IsLooping() { return m_pAudioBuffer && m_pAudioBuffer->IsLooping(); }
        float GetProgress();
        void PauseSound();

    protected:
        //Не позволяет использовать конструктор по-умолчанию из вне
        SoundProcess();

        virtual void OnInit();
        virtual void OnUpdate(double dt);

    protected:
        std::shared_ptr<ResHandle> m_pHandle;
        std::shared_ptr<IAudioBuffer> m_pAudioBuffer;

        int m_volume;
        bool m_isLooping;

    };

}