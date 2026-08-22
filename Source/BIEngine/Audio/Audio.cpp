#include "Audio.h"

#include "fmod/fmodAudioManager.h"

namespace BIEngine {

AudioManager* CreateAudioManager()
{
    AudioManager* pAudioManager = new fmodAudioManager();
    return pAudioManager;
}


} // namespace BIEngine
