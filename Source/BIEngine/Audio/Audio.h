#pragma once

#include "../ResourceCache/ResCache.h"

namespace BIEngine
{

	class SoundResourceExtraData;

	enum SoundType
	{
		SOUND_TYPE_FIRST,
		SOUND_TYPE_MP3 = SOUND_TYPE_FIRST,
		SOUND_TYPE_WAVE,
		SOUND_TYPE_MIDI,
		SOUND_TYPE_OGG,

		//Счетчик наших доступных форматов. Должен быть последним.
		SOUND_TYPE_COUNT,
		SOUND_TYPE_UNKNOWN,
	};

	extern char* gSoundExtentions[];

	//Буффер загруженного аудио
	class IAudioBuffer
	{
	public:
		virtual ~IAudioBuffer() { }

		virtual std::shared_ptr<ResHandle> GetResource() = 0;
		virtual bool OnRestore() = 0;

		virtual bool Play(int volume, bool looping) = 0;
		virtual bool Pause() = 0;
		virtual bool Stop() = 0;
		virtual bool Resume() = 0;

		virtual bool TogglePause() = 0;
		virtual bool IsPlaying() = 0;
		virtual bool IsLooping() const = 0;
		virtual void SetVolume(int volume) = 0;
		virtual void SetPosition(unsigned long newPosition) = 0;
		virtual int GetVolume() const = 0;
		virtual float GetProgress() = 0;
	};

	//Реализация аудио-буфера реализаованная над системой ресурсов движка
	class AudioBuffer : public IAudioBuffer
	{
	public:
		virtual std::shared_ptr<ResHandle> GetResource() { return m_pResource; }
		virtual bool IsLooping() const { return m_isLooping; }
		virtual int GetVolume() const { return m_volume; }

	protected:
		AudioBuffer(std::shared_ptr<ResHandle >resource)
		{
			m_pResource = resource;
			m_isPaused = false;
			m_isLooping = false;
			m_volume = 0;
		}

		std::shared_ptr<ResHandle> m_pResource;

	protected:
		bool m_isPaused;
		bool m_isLooping;
		int m_volume;
	};

	class IAudio
	{
	public:
		virtual bool Active() = 0;

		virtual IAudioBuffer* InitAudioBuffer(std::shared_ptr<ResHandle> handle) = 0;
		virtual void ReleaseAudioBuffer(IAudioBuffer* audioBuffer) = 0;

		virtual void StopAllSounds() = 0;
		virtual void PauseAllSounds() = 0;
		virtual void ResumeAllSounds() = 0;

		virtual bool Initialize() = 0;
		virtual void Shutdown() = 0;
	};

	//Главная аудио система, управляющая аудио-буферами
	class Audio : public IAudio
	{
	public:
		Audio();
		~Audio() { Shutdown(); }
		virtual void StopAllSounds();
		virtual void PauseAllSounds();
		virtual void ResumeAllSounds();

		virtual void Shutdown();
		static bool HasSoundCard();
		bool IsPaused() { return m_allPaused; }

	protected:
		typedef std::list<IAudioBuffer*> AudioBufferList;

		AudioBufferList m_allSamples;
		bool m_allPaused;
		bool m_initialized;
	};

	extern Audio* g_pAudio;

}

