#pragma once

#include "AudioManager.h"

namespace BIEngine {

// Главная аудио система, управляющая аудио-буферами

extern AudioManager* g_pAudio;

extern AudioManager* CreateAudioManager();

} // namespace BIEngine
