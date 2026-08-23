#include "fmodAudioGroupManager.h"

#include "fmodAudioGroup.h"
#include "fmodAudioManager.h"
#include "../Audio.h"

namespace BIEngine
{

    SharedPtr<IAudioGroup> fmodAudioGroupManager::CreateMasterAudioGroup()
    {
        if (g_pAudio == nullptr)
        {
            return nullptr;
        }

        fmodAudioManager* pAudioManager = static_cast<fmodAudioManager*>(g_pAudio);

        FMOD::ChannelGroup* pGroup;
        if (pAudioManager->GetSystem()->getMasterChannelGroup(&pGroup) != FMOD_OK)
        {
            return nullptr;
        }

        return MakeShared<fmodAudioGroup>(pGroup);
    }

    SharedPtr<IAudioGroup> fmodAudioGroupManager::CreateAudioGroup()
    {
        if (g_pAudio == nullptr)
        {
            return nullptr;
        }

        fmodAudioManager* pAudioManager = static_cast<fmodAudioManager*>(g_pAudio);

        FMOD::ChannelGroup* pGroup;
        if (pAudioManager->GetSystem()->createChannelGroup(0, &pGroup) != FMOD_OK)
        {
            return nullptr;
        }

        return MakeShared<fmodAudioGroup>(pGroup);
    }

}