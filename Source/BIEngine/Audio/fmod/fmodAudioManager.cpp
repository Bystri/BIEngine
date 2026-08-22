#include "fmodAudioManager.h"

#include "fmodAudioBuffer.h"
#include "../../Utilities/Logger.h"

namespace BIEngine
{

    bool fmodAudioManager::Initialize()
    {
        if (m_initialized)
        {
            return true;
        }

        Shutdown();

        FMOD_RESULT result = FMOD::System_Create(&m_pFMODSystem);
        if (result != FMOD_OK || !m_pFMODSystem)
        {
            Logger::WriteErrorLog(FMOD_ErrorString(result));
            return false;
        }

        result = m_pFMODSystem->init(100, FMOD_INIT_NORMAL, 0);
        if (result != FMOD_OK)
        {
            Logger::WriteErrorLog(FMOD_ErrorString(result));
            m_pFMODSystem->release();
            m_pFMODSystem = nullptr;
            return false;
        }

        m_initialized = true;

        return true;
    }

    void fmodAudioManager::OnUpdate()
    {
        if (m_pFMODSystem) {
            m_pFMODSystem->update();
        }
    }

    void fmodAudioManager::Shutdown()
    {
        if (m_pFMODSystem) {
            AudioManager::Shutdown();
            m_pFMODSystem->release();
            m_pFMODSystem = nullptr;
        }

        m_initialized = false;
    }

    IAudioBuffer* fmodAudioManager::InitAudioBuffer(char* buffer, unsigned int bufferSize, IAudioBuffer::LoadType loadType)
    {
        if (!m_pFMODSystem || !buffer)
        {
            return nullptr;
        }

        fmodAudioBuffer* pAudioBuffer = new fmodAudioBuffer(m_pFMODSystem, buffer, bufferSize, loadType);
        if (!pAudioBuffer->IsValid()) {
            delete pAudioBuffer;
            return nullptr;
        }

        m_allBuffers.PushFront(pAudioBuffer);

        return pAudioBuffer;
    }

    void fmodAudioManager::ReleaseAudioBuffer(IAudioBuffer* pSampleHandle)
    {
        if (!pSampleHandle) {
            return;
        }

        pSampleHandle->StopAllSounds();
        pSampleHandle->Release();
        m_allBuffers.Remove(pSampleHandle);
        delete pSampleHandle;
    }

}