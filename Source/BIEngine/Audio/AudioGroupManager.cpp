#include "AudioGroupManager.h"

#include "../Utilities/Logger.h"

namespace BIEngine
{
    void AudioGroupManager::Init()
    {
        m_masterGroup = CreateMasterAudioGroup();
    }

    void AudioGroupManager::AddAudioGroup(const char* groupId)
    {
        auto itr = m_audioGroups.Find(groupId);
        if (itr != m_audioGroups.End())
        {
            Logger::WriteWarningLog("AudioGroup with id %s already exists!", groupId);
            return;
        }

        m_audioGroups.Emplace(groupId, CreateAudioGroup());
    }

    WeakPtr<IAudioGroup> AudioGroupManager::GetGroup(const char* groupId)
    {
        auto itr = m_audioGroups.Find(groupId);
        if (itr == m_audioGroups.End())
        {
            Logger::WriteWarningLog("AudioGroup with id %s doesn't exist!", groupId);
            return WeakPtr<IAudioGroup>();
        }

        return itr->second;
    }

}