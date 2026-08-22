#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "SoundLoader.h"
#include "../Audio/Audio.h"

namespace BIEngine {

    SoundBufferData::~SoundBufferData()
    {
        if (m_pAudioBuffer)
        {
            g_pAudio->ReleaseAudioBuffer(m_pAudioBuffer);
        }
    }

    bool OggResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
    {
        SharedPtr<SoundBufferData> pExtra = MakeShared<SoundBufferData>();

        pExtra->m_pAudioBuffer = g_pAudio->InitAudioBuffer(rawBuffer, rawSize, IAudioBuffer::LoadType::STREAMING);

        pHandle->SetExtra(pExtra);

        return true;
    }

    bool WavResourceLoader::LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle)
    {
        SharedPtr<SoundBufferData> pExtra = MakeShared<SoundBufferData>();

        pExtra->m_pAudioBuffer = g_pAudio->InitAudioBuffer(rawBuffer, rawSize, IAudioBuffer::LoadType::DECOMPRESS_ON_LOAD);

        pHandle->SetExtra(pExtra);

        return true;
    }

} // namespace BIEngine
