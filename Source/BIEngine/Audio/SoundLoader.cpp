//#include <io.h>
//#include <fcntl.h>
//#include <sys/stat.h>
//#include <sys/types.h>

#include    <vorbis/codec.h>       
#include    <vorbis/vorbisfile.h>       

#include "SoundLoader.h"
#include "../Audio/Audio.h"

namespace BIEngine
{

	std::shared_ptr<IResourceLoader> CreateOGGResourceLoader()
	{
		return std::make_shared<OggResourceLoader>();
	}

	std::shared_ptr<IResourceLoader> CreateWavResourceLoader()
	{
		return std::make_shared<WavResourceLoader>();
	}

}




