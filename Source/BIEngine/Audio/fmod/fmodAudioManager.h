#pragma once

#include <fmod.hpp>
#include <fmod_errors.h>

#include "../AudioManager.h"

namespace BIEngine
{

    class fmodAudioManager : public AudioManager {
    public:
        virtual ~fmodAudioManager() { Shutdown(); }

        virtual bool Active() { return m_pFMODSystem != nullptr; }

        virtual IAudioBuffer* InitAudioBuffer(SharedPtr<ResHandle> pHandle, IAudioBuffer::LoadType loadType) override;
        virtual void ReleaseAudioBuffer(IAudioBuffer* pAudioBuffer) override;

        virtual void Shutdown() override;
        virtual bool Initialize() override;

        virtual void OnUpdate() override;

    protected:
        FMOD::System* m_pFMODSystem = nullptr;
    };

}