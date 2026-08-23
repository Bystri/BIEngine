#include "fmodAudioGroup.h"

#include <fmod_errors.h>

#include "fmodAudioSound.h"

namespace BIEngine
{
    fmodAudioGroup::fmodAudioGroup(FMOD::ChannelGroup* pChannel)
        : m_pChannelGroup(pChannel)
    {

    }

    void fmodAudioGroup::ApplyToSound(IAudioSound* pAudioSound)
    {
        if (pAudioSound == nullptr)
        {
            return;
        }

        fmodAudioSound* const pFmodSound = static_cast<fmodAudioSound*>(pAudioSound);
        pFmodSound->GetChannel()->setChannelGroup(m_pChannelGroup);
    }

    void fmodAudioGroup::SetVolume(float volume)
    {
        m_pChannelGroup->setVolume(volume);
    }

    float fmodAudioGroup::GetVolume() const
    {
        float volume = 0.0f;

        if (m_pChannelGroup->getVolume(&volume) != FMOD_OK)
        {
            return 0.0f;
        }

        return volume;
    }

}