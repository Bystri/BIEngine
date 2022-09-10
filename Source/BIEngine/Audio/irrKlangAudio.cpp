#include "irrKlangAudio.h"

#include "soundLoader.h"

namespace BIEngine
{

	bool irrKlangAudio::Initialize()
	{
		if (m_initialized)
			return true;

		m_allSamples.clear();

		if (m_pSoundEngine) {
			m_pSoundEngine->drop();
			m_pSoundEngine = nullptr;
		}

		m_pSoundEngine = irrklang::createIrrKlangDevice();

		if (!m_pSoundEngine)
			return false;

		m_initialized = true;

		return true;
	}

	void irrKlangAudio::Shutdown()
	{
		if (m_initialized)
		{
			Audio::Shutdown();
			if (m_pSoundEngine) {
				m_pSoundEngine->drop();
				m_pSoundEngine = nullptr;
			}
			m_initialized = false;
		}
	}

	IAudioBuffer* irrKlangAudio::InitAudioBuffer(std::shared_ptr<ResHandle> pResHandle)
	{
		if (!m_pSoundEngine)
			return nullptr;

		IAudioBuffer* pAudioBuffer = new irrKlangAudioBuffer(m_pSoundEngine, pResHandle);
		m_allSamples.push_front(pAudioBuffer);

		return pAudioBuffer;

	}

	void irrKlangAudio::ReleaseAudioBuffer(IAudioBuffer* pSampleHandle)
	{
		pSampleHandle->Stop();
		m_allSamples.remove(pSampleHandle);
	}

	irrKlangAudioBuffer::irrKlangAudioBuffer(irrklang::ISoundEngine* pSoundEngine, std::shared_ptr<ResHandle> pResource)
		: AudioBuffer(pResource)
		, m_pSoundEngine(pSoundEngine)
		, m_pSound(nullptr)
	{
		m_pSoundSource = m_pSoundEngine->getSoundSource(pResource->GetName().c_str(), false);

		if (!m_pSoundSource)
			m_pSoundSource = m_pSoundEngine->addSoundSourceFromMemory(pResource->Buffer(), pResource->Size(), pResource->GetName().c_str());
	}

	bool irrKlangAudioBuffer::Play(int volume, bool looping)
	{
		if (!g_pAudio->Active())
			return false;

		Stop();

		m_volume = volume;
		m_isLooping = looping;

		if (!m_pSoundEngine || !m_pSoundSource)
			return false;

		SetVolume(volume);

		m_pSoundEngine->play2D(m_pSoundSource->getName(), looping, false, true);

		return m_pSound;

	}

	bool irrKlangAudioBuffer::Stop()
	{
		if (!g_pAudio->Active() || !m_pSound)
			return false;

		m_isPaused = true;
		m_pSound->stop();
		m_pSound->drop();
		m_pSound = nullptr;

		return true;
	}

	bool irrKlangAudioBuffer::Pause()
	{
		if (!g_pAudio->Active() || !m_pSound)
			return false;

		m_isPaused = true;
		m_pSound->setIsPaused();
		m_pSound->setPlayPosition(0);
		return true;
	}

	bool irrKlangAudioBuffer::Resume()
	{
		m_isPaused = false;
		return Play(GetVolume(), IsLooping());
	}

	bool irrKlangAudioBuffer::TogglePause()
	{
		if (!g_pAudio->Active())
			return false;

		if (m_isPaused)
			Resume();
		else
			Pause();

		return true;
	}

	bool irrKlangAudioBuffer::IsPlaying()
	{
		if (!g_pAudio->Active() || !m_pSound)
			return false;

		bool b = m_pSound->isFinished();
		return !m_pSound->isFinished() && !m_pSound->getIsPaused();
	}

	//Громкость может быть в диапазоне от 0 до 100
	void irrKlangAudioBuffer::SetVolume(int volume)
	{
		if (!g_pAudio->Active() || !m_pSound)
			return;

		assert(volume >= 0 && volume <= 100 && "Volume must be a number between 0 and 100");

		float coeff = (float)volume / 100.0f;
		m_pSound->setVolume(coeff);
	}

	void irrKlangAudioBuffer::SetPosition(unsigned long newPosition)
	{
		if (!g_pAudio->Active() || !m_pSound)
			return;

		m_pSound->setPlayPosition(newPosition);
	}

	bool irrKlangAudioBuffer::OnRestore()
	{
		return true;
	}

	//Возвращает долю величины уже проигранной части музыки. Возвращается в диапазоне от 0.0 до 1.0
	float irrKlangAudioBuffer::GetProgress()
	{
		if (!g_pAudio->Active() || !m_pSound)
			return 0.f;

		float progress = m_pSound->getPlayPosition();
		float length = m_pSound->getPlayLength();

		return progress / length;
	}

}


