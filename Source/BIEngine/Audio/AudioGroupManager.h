#pragma once

#include "AudioGroup.h"
#include "../StdLib/String.h"
#include "../StdLib/HashMap.h"
#include "../StdLib/SharedPtr.h"

namespace BIEngine
{

    class IAudioGroupManager
    {
    public:
        IAudioGroupManager() = default;
        virtual ~IAudioGroupManager() = default;

        IAudioGroupManager(const IAudioGroupManager&) = delete;
        IAudioGroupManager& operator=(const IAudioGroupManager&) = delete;

        virtual void Init() = 0;

        virtual void AddAudioGroup(const char* groupId) = 0;

        virtual WeakPtr<IAudioGroup> GetMasterGroup() = 0;
        virtual WeakPtr<IAudioGroup> GetGroup(const char* groupId) = 0;

    protected:
        virtual SharedPtr<IAudioGroup> CreateMasterAudioGroup() = 0;
        virtual SharedPtr<IAudioGroup> CreateAudioGroup() = 0;
    };

    class AudioGroupManager : public IAudioGroupManager
    {
    public:
        virtual void Init() override;

        virtual void AddAudioGroup(const char* groupId) override;

        virtual WeakPtr<IAudioGroup> GetMasterGroup() override { return m_masterGroup; }
        virtual WeakPtr<IAudioGroup> GetGroup(const char* groupId) override;

    private:
        SharedPtr<IAudioGroup> m_masterGroup;
        HashMap<String, SharedPtr<IAudioGroup>> m_audioGroups;
    };

}