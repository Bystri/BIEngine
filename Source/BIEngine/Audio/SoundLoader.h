#pragma once

#include "AudioBuffer.h"
#include "../ResourceCache/ResCache.h"

namespace BIEngine {

class SoundBufferData : public IResourceExtraData {
    friend class OggResourceLoader;
    friend class WavResourceLoader;

public:
    SoundBufferData() = default;

    SoundBufferData(const SoundBufferData& orig) = delete;
    SoundBufferData& operator=(const SoundBufferData& orig) = delete;

    virtual ~SoundBufferData() override;

    virtual String ToString() override { return "SoundBufferData"; }

    IAudioBuffer* GetAudioBuffer() const { return m_pAudioBuffer; }

protected:
    IAudioBuffer* m_pAudioBuffer;
};

class OggResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return false; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle) override;

   virtual String GetPattern() { return "*.ogg"; }
};

class WavResourceLoader : public IResourceLoader {
public:
   virtual bool UseRawFile() { return false; }

   virtual bool DiscardRawBufferAfterLoad() { return false; }

   virtual unsigned int GetLoadedResourceSize(char* rawBuffer, unsigned int rawSize) { return 0u; }

   virtual bool LoadResource(char* rawBuffer, unsigned int rawSize, SharedPtr<ResHandle> pHandle) override;

   virtual String GetPattern() { return "*.wav"; }
};

} // namespace BIEngine
