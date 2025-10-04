#pragma once

#include "Utils.h"

#include "raylib.h"

namespace BlastOff
{
	struct SoundLoader
	{
		~SoundLoader();
		const Sound* LazyLoadSound(const char* const resourcePath);

	private:
		unordered_map<string, Sound> m_CachedValues = {};
		const Sound* LoadAndInsert(const char* const resourcePath);
	};

	struct MusicLoader
	{
		~MusicLoader();
		const Music* LazyLoadMusic(const char* const resourcePath);

	private:
		unordered_map<string, Music> m_CachedValues = {};
		const Music* LoadAndInsert(const char* const resourcePath);
	};

	struct MusicLoop
	{
		MusicLoop(
			const Music* song,
			const float loopStart,
			const float loopEnd
		);
		static unique_ptr<MusicLoop> LoadFromPath(
			const char* const resourcePath,
			const float loopStart,
			const float loopEnd,
			MusicLoader* const musicLoader
		);

		void SetVolume(const float volume);
		void Play();
		void Update();

	private:
		const Music* m_Song = nullptr;

		const float m_LoopStart = 0;
		const float m_LoopEnd = 0;
	};
}
