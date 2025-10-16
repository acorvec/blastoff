#pragma once

#include "raylib.h"

#include "Utils.h"
#include "ProgramConstants.h"
#include "Debug.h"
#include "Graphics.h"
#include "Game.h"
#include "Sound.h"
#include "Enums.h"
#include "Settings.h"
#include <chrono>

namespace BlastOff
{
	struct Program
	{
		Program();
		~Program();

		void RunLoopIteration();
		bool IsRunning() const;

	private:
		using State = ProgramState;

		static const inline ProgramConstants c_Config;
		static const bool c_DrawFPS;

		bool m_IsRunning = true;
		bool m_ShouldCloseAfterFrame = false;
        bool m_IsMuted = false;
		bool m_GameShouldReset = false;
        bool m_CutsceneShouldReset = false;

		Font m_Font = { 0 };
		State m_State = State::None;
		optional<State> m_PendingStateChange = std::nullopt;
        Vector2f m_CameraPosition = Vector2f::Zero();

		ImageTextureLoader m_ImageTextureLoader;
		SoundLoader m_SoundLoader;
		MusicLoader m_MusicLoader;

		unique_ptr<Settings> m_Settings = nullptr;
		unique_ptr<TextTextureLoader> m_TextTextureLoader = nullptr;
        unique_ptr<CoordinateTransformer> m_CoordinateTransformer = nullptr;
        unique_ptr<InputManager> m_InputManager = nullptr;
        unique_ptr<CameraEmpty> m_CameraEmpty = nullptr;
		unique_ptr<RayWindow> m_Window = nullptr;
		unique_ptr<MusicLoop> m_BackgroundMusicLoop = nullptr;
		unique_ptr<Game> m_Game = nullptr;
        unique_ptr<Cutscene> m_Cutscene = nullptr;
		unique_ptr<MainMenu> m_MainMenu = nullptr;
		unique_ptr<SettingsMenu> m_SettingsMenu = nullptr;

		time_point<high_resolution_clock> m_FrameStartTime = high_resolution_clock::now();

#if COMPILE_CONFIG_DEBUG
		int m_MostRecentFramerateSet = 0;
#endif
		void EndFrame();
		void Update();
		void Draw() const;

		bool ShouldShowCutscene() const;
		void MuteOrUnmute();
		
		int CalculateNormalFramerate() const;
		int MultiplyFramerate(const float multiplier) const;
		void SetFramerate(const int framerate);
		static void DrawFramerate();

		void InitializeGame();
		void InitializeMainMenu();
		void InitializeCutscene();
		void InitializeSettingsMenu();
	};
}
