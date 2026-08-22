#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>

#include "../AudioBuffer.h"
#include "../../StdLib/SharedPtr.h"
#include "../../ResourceCache/ResCache.h"

namespace BIEngine
{

    class fmodAudioBuffer : public AudioBuffer {
    public:
        fmodAudioBuffer(FMOD::System* pSoundEngine, SharedPtr<ResHandle> pResource, IAudioBuffer::LoadType loadType);

        bool IsValid() const { return m_pFMODSound != nullptr; }

        virtual IAudioSound* Play(float volume, bool looping) override;
        virtual bool Release() override;

    private:
        FMOD::System* m_pFMODSystem = nullptr;
        FMOD::Sound* m_pFMODSound = nullptr;
        SharedPtr<ResHandle> m_pResource;
    };

}