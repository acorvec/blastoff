#pragma once

#include "raylib.h"

#include "Utils.h"
#include "ProgramConfiguration.h"
#include "Debug.h"
#include "Graphics.h"
#include "Game.h"
#include "Sound.h"
#include "Enums.h"
#include "MainMenu.h"

namespace BlastOff
{
	struct Program
	{
		Program();
		~Program();

		void RunLoopIteration();
		bool IsRunning() const;

		void Update();
		void Draw() const;
		void EndFrame();

	private:
		using State = ProgramState;

		static const inline ProgramConfiguration c_Config;

		bool m_IsRunning = true;
        bool m_IsMuted = false;
		bool m_GameShouldReset = false;

		Font m_Font = { 0 };
		State m_State = State::None;

		ImageTextureLoader m_ImageTextureLoader;
		SoundLoader m_SoundLoader;
		MusicLoader m_MusicLoader;

		unique_ptr<TextTextureLoader> m_TextTextureLoader = nullptr;
		unique_ptr<RayWindow> m_Window = nullptr;
		unique_ptr<MusicLoop> m_BackgroundMusicLoop = nullptr;
		unique_ptr<Game> m_Game = nullptr;
		unique_ptr<MainMenu> m_MainMenu = nullptr;

#if COMPILE_CONFIG_DEBUG
		int m_MostRecentFramerateSet = 0;
#endif

		int CalculateNormalFramerate() const;
		int MultiplyFramerate(const float multiplier) const;
		void SetFramerate(const int framerate);

		void InitializeGame();
	};
}
