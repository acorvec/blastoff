#include "Program.h"
#include "Game.h"
#include "Logging.h"
#include <cstddef>
#include <stdexcept>

namespace BlastOff
{
	Program::Program()
	{
		const auto logInitialMessage =
			[]()
			{
				const char* const message = "Starting program.";
				Logging::Log(message);
			};

		const auto initializeGraphics =
			[&, this]()
			{
				const Vector2i windowSize = c_Config.GetDefaultWindowSize();
				const string windowName = c_Config.CalculateBuildString();

				m_Window = std::make_unique<RayWindow>(windowSize, windowName);

				const int normalFramerate = CalculateNormalFramerate();
				SetFramerate(normalFramerate);

				const string& fontFace = c_Config.GetFontFace();
				const string fontPath = GetFontPath(fontFace.c_str(), "ttf");
				const int fontSize = c_Config.GetFontRenderSize();

				constexpr int codepointCount = 0;
				m_Font = LoadFontEx(
					fontPath.c_str(), 
					fontSize, 
					nullptr, 
					codepointCount
				);
				m_TextTextureLoader = 
				{
					std::make_unique<TextTextureLoader>(&m_Font)
				};
			};

		const auto initializeSound =
			[this]()
			{
				const bool isSoundEnabled = c_Config.GetSoundEnabled();
				if (isSoundEnabled)
					InitAudioDevice();
			};

		const auto initializeBackgroundMusic =
			[this]()
			{
				const bool isSoundEnabled = c_Config.GetSoundEnabled();
				if (!isSoundEnabled)
					return;

				const string& resourcePath =
				{
					c_Config.GetBackgroundMusicPath()
				};
				const float loopStart = c_Config.GetBackgroundMusicLoopStart();
				const float loopEnd = c_Config.GetBackgroundMusicLoopEnd();
				const float volume = c_Config.GetBackgroundMusicVolume();

				m_BackgroundMusicLoop = MusicLoop::LoadFromPath(
					resourcePath.c_str(),
					loopStart,
					loopEnd,
					&m_MusicLoader
				);

				m_BackgroundMusicLoop->SetVolume(volume);
				m_BackgroundMusicLoop->Play();
			};

		const auto disableEscapeKey =
			[]()
			{
				// by default, Raylib quits
				// when the user presses the Escape key.
				// this is bad, so i've decided to disable it
				if (!c_Config.GetEscapeKeyEnabled())
					SetExitKey(KEY_NULL);
			};

		Logging::Initialize(&c_Config);
		logInitialMessage();
		
		initializeGraphics();
		initializeSound();
		initializeBackgroundMusic();
		disableEscapeKey();
		InitializeMainMenu();
		InitializeGame();

		m_State = State::MainMenu;
	}

	Program::~Program()
	{
		if (IsAudioDeviceReady())
			CloseAudioDevice();

		if (m_Font.texture.id)
			UnloadFont(m_Font);
	}

	bool Program::IsRunning() const
	{
		return m_IsRunning;
	}

	void Program::RunLoopIteration()
	{
		Update();
		Draw();
		EndFrame();

		if (m_GameShouldReset)
		{
			InitializeGame();
			m_GameShouldReset = false;
		}

		const auto reinitializeRelevantObject = 
			[this](const State state)
			{
				switch (state)
				{
					case State::Game:
						InitializeGame();
						break;

					case State::MainMenu:
						InitializeMainMenu();
						break;

					default:
						throw std::runtime_error(
							"Program::Update() failed: "
							"Invalid value of ProgramState enum."
						);
				}
			};

		const auto handleStateChange = 
			[&, this]()
			{
				reinitializeRelevantObject(m_State);
				reinitializeRelevantObject(*m_PendingStateChange);
				
				m_State = *m_PendingStateChange;
				m_PendingStateChange = std::nullopt;
			};
			
		if (m_PendingStateChange)
			handleStateChange();
	}

	void Program::Update()
	{
		const auto updateControlQ =
			[this]()
			{
				if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_Q))
					m_IsRunning = false;
			};

#if COMPILE_CONFIG_DEBUG
		const auto activateFastMode =
			[&, this]()
			{
				const float multiplier = c_Config.GetFastModeMultiplier();
				const int newTargetFramerate = MultiplyFramerate(multiplier);
				SetFramerate(newTargetFramerate);
			};

		const auto activateSlowMode =
			[&, this]()
			{
				const float multiplier = c_Config.GetSlowModeMultiplier();
				const int newTargetFramerate = MultiplyFramerate(multiplier);
				SetFramerate(newTargetFramerate);
			};

		const auto activateRegularSpeedMode =
			[this]()
			{
				const float multiplier = c_Config.GetNormalModeMultiplier();
				const int newTargetFramerate = MultiplyFramerate(multiplier);
				SetFramerate(newTargetFramerate);
			};


		const auto updateSpeedKeys =
			[&, this]()
			{
				const int fastModeKey = c_Config.GetFastModeKey();
				const int slowModeKey = c_Config.GetSlowModeKey();

				if (IsKeyDown(fastModeKey))
					activateFastMode();
				else if (IsKeyDown(slowModeKey))
					activateSlowMode();
				else
					activateRegularSpeedMode();
			};
#endif

		m_Window->Update();

		if (m_State == State::MainMenu)
			m_MainMenu->Update();
		else if (m_State == State::Game)
			m_Game->Update();
		
		const bool isSoundEnabled = c_Config.GetSoundEnabled();
		if (isSoundEnabled)
			m_BackgroundMusicLoop->Update();

		const bool controlQEnabled = c_Config.GetControlQEnabled();
		if (controlQEnabled)
			updateControlQ();

#if COMPILE_CONFIG_DEBUG
		if (c_Config.GetDebugToolsEnabled())
			updateSpeedKeys();
#endif
	}

	void Program::Draw() const
	{
		BeginDrawing();

		const Colour4i voidColour = c_Config.GetVoidColour();
		ClearBackground(voidColour.ToRayColour());

		if (m_State == State::MainMenu)
			m_MainMenu->Draw();
		else if (m_State == State::Game)
			m_Game->Draw();

		EndDrawing();
	}

	void Program::EndFrame()
	{
		if (WindowShouldClose())
			m_IsRunning = false;
	}

	int Program::CalculateNormalFramerate() const
	{
#if COMPILE_CONFIG_DEBUG
		const float framerateMultiplier =
		{
			c_Config.GetNormalModeMultiplier()
		};
		return MultiplyFramerate(framerateMultiplier);
#else
		return c_Config.GetTargetFramerate();
#endif
	}

	int Program::MultiplyFramerate(const float multiplier) const
	{
		const int targetFramerate = c_Config.GetTargetFramerate();
		const float result = targetFramerate * multiplier;
		return (int)roundf(result);
	}

	void Program::SetFramerate(const int framerate) 
	{
#if COMPILE_CONFIG_DEBUG
		if (framerate != m_MostRecentFramerateSet)
			SetTargetFPS(framerate);

		m_MostRecentFramerateSet = framerate;
#else
		SetTargetFPS(framerate);
#endif
	}

	void Program::InitializeGame()
	{
		const auto resetCallback =
			[this]()
			{
				m_GameShouldReset = true;
			};

        const auto muteUnmuteCallback = 
            [this]()
            {
                m_IsMuted = !m_IsMuted;

                if (m_IsMuted)
                    SetMasterVolume(0);
                else
                    SetMasterVolume(1);
            };

		const auto exitCallback = 
			[this]()
			{
				m_PendingStateChange = State::MainMenu;
			};

		m_Game = std::make_unique<PlayableGame>(
            &m_IsMuted,
			&c_Config,
			&m_ImageTextureLoader,
			m_TextTextureLoader.get(),
			&m_SoundLoader,
            muteUnmuteCallback,
			resetCallback,
			exitCallback,
			&m_Font,
			m_Window->GetPosition(),
			m_Window->GetSize()
		);
	}

	void Program::InitializeMainMenu()
	{
		const auto settingsCallback = 
			[this]()
			{
				// TODO: add settings menu functionality
				m_PendingStateChange = State::SettingsMenu;
			};

		const auto playCallback =
			[this]()
			{
				m_PendingStateChange = State::Game;
			};

		m_MainMenu = std::make_unique<MainMenu>(
			&c_Config,
			&m_ImageTextureLoader,
			m_TextTextureLoader.get(),
			&m_SoundLoader,
			playCallback,
			settingsCallback,
			&m_Font,
			m_Window->GetPosition(),
			m_Window->GetSize()
		);
	}
}
