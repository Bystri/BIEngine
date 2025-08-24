#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

#include "SoundLoader.h"
#include "../Audio/Audio.h"

namespace BIEngine {

SharedPtr<IResourceLoader> CreateOGGResourceLoader()
{
   return MakeShared<OggResourceLoader>();
}

SharedPtr<IResourceLoader> CreateWavResourceLoader()
{
   return MakeShared<WavResourceLoader>();
}

} // namespace BIEngine
