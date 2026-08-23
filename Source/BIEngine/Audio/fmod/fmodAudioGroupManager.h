#pragma once

#include "../AudioGroupManager.h"

namespace BIEngine
{

    class fmodAudioGroupManager : public AudioGroupManager
    {
    protected:
        virtual SharedPtr<IAudioGroup> CreateMasterAudioGroup() override;
        virtual SharedPtr<IAudioGroup> CreateAudioGroup() override;
    };

}