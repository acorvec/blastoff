#include "Sound.h"
#include "Logging.h"
#include "OperatingSystem.h"

namespace BlastOff
{
	namespace
	{
		void OnSoundLoadingError(const string& loadingPath)
		{
			const string message = 
			{
				"Unable to load Sound from sound file "
				"at path \"" + loadingPath + "\"."
			};
			Logging::Log(message.c_str());
			BreakProgram();
		}

		void ThrowMusicLoadingException(const string& loadingPath)
		{
			const string message = 
			{
				"Unable to load Music from sound file "
				"at path \"" + loadingPath + "\"."
			};
			Logging::Log(message.c_str());
			BreakProgram();
		}

		Sound LoadWAVSound(const char* const resourcePath)
		{
			const static string start = string("resource/wav/");
			const string resultingPath = start + resourcePath;
			const char* const cString = resultingPath.c_str();

			const Sound result = LoadSound(cString);
			if (!result.frameCount)
			{
				OnSoundLoadingError(resultingPath);
				return result;
			}
			else
				return result;
		}

		Music LoadOGGMusic(const char* const resourcePath)
		{
			const static string start = string("resource/ogg/");
			const string resultingPath = start + resourcePath;
			const char* const cString = resultingPath.c_str();

			const Music result = LoadMusicStream(cString);
			if (!result.frameCount)
			{
				ThrowMusicLoadingException(resultingPath);
				return result;
			}
			else
				return result;
		}
	}

	SoundLoader::~SoundLoader()
	{
		for (const auto& [name, sound] : m_CachedValues)
		{
			(void)name;
			UnloadSound(sound);
		}
	}

	const Sound* SoundLoader::LazyLoadSound(const char* const resourcePath)
	{
		const auto getCachedValue =
			[&, this]() -> const Sound*
			{
				try
				{
					return &m_CachedValues.at(resourcePath);
				}
				catch (const std::out_of_range& exception)
				{
					(void)exception;
					return nullptr;
				}
			};

		const auto result = getCachedValue();
		if (!result)
			return LoadAndInsert(resourcePath);
		else
			return result;
	}

	const Sound* SoundLoader::LoadAndInsert(const char* const resourcePath)
	{
		const Sound result = LoadWAVSound(resourcePath);
		m_CachedValues.insert({ resourcePath, result });

		return &m_CachedValues.at(resourcePath);
	}


	MusicLoader::~MusicLoader()
	{
		for (const auto& [name, music] : m_CachedValues)
		{
			(void)name;
			UnloadMusicStream(music);
		}
	}

	const Music* MusicLoader::LazyLoadMusic(const char* const resourcePath)
	{
		const auto getCachedValue =
			[&, this]() -> const Music*
			{
				try
				{
					return &m_CachedValues.at(resourcePath);
				}
				catch (const std::out_of_range& exception)
				{
					(void)exception;
					return nullptr;
				}
			};

		const auto result = getCachedValue();
		if (!result)
			return LoadAndInsert(resourcePath);
		else
			return result;
	}

	const Music* MusicLoader::LoadAndInsert(const char* const resourcePath)
	{
		const Music result = LoadOGGMusic(resourcePath);
		m_CachedValues.insert({ resourcePath, result });

		return &m_CachedValues.at(resourcePath);
	}




	MusicLoop::MusicLoop(
		const Music* song,
		const float loopStart,
		const float loopEnd
	) :
		m_Song(song),
		m_LoopStart(loopStart),
		m_LoopEnd(loopEnd)
	{

	}

	unique_ptr<MusicLoop> MusicLoop::LoadFromPath(
		const char* const resourcePath,
		const float loopStart,
		const float loopEnd,
		MusicLoader* const musicLoader
	)
	{
		const Music* const musicStream = 
		{
			musicLoader->LazyLoadMusic(resourcePath)
		};
		return std::make_unique<MusicLoop>(
			musicStream,
			loopStart,
			loopEnd
		);
	}

	void MusicLoop::SetVolume(const float volume)
	{
		SetMusicVolume(*m_Song, volume);
	}

	void MusicLoop::Play()
	{
		PlayMusicStream(*m_Song);
	}

	void MusicLoop::Update()
	{
		const float playTime = GetMusicTimePlayed(*m_Song);
		if (playTime >= m_LoopEnd)
			SeekMusicStream(*m_Song, m_LoopStart);

		// raysan5 needs to add documentation for this lmao
		UpdateMusicStream(*m_Song);
	}
}
