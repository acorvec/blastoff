#include "Program.h"
#include "GUI.h"
#include "Game.h"
#include "Graphics.h"
#include "Logging.h"
#include "ProgramConstants.h"
#include "Settings.h"
#include "raylib.h"

#include <memory>
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

		const auto applySettings = 
			[this]()
			{
				const Vector2f aspectRatio = c_Config.GetDefaultAspectRatio();
				m_Settings = Settings::LoadOrDefault(aspectRatio);

				const Vector2i windowSize = m_Settings->GetWindowSize();
				const Vector2i windowPosition = 
				{
					m_Settings->GetWindowPosition()
				};
				m_Window->SetSize(windowSize);
				m_Window->SetPosition(windowPosition);
			};

		const auto pauseForOneFrame = 
			[this]()
			{
				BeginDrawing();

				const Colour4i voidColour = c_Config.GetVoidColour();
				ClearBackground(voidColour.ToRayColour());

				EndDrawing();
			};

		const auto initializeGraphics =
			[&, this]()
			{
				constexpr Vector2i dummyWindowSize = { 0, 0 };
				const string windowName = c_Config.CalculateBuildString();

				m_Window = std::make_unique<RayWindow>(
					dummyWindowSize, 
					windowName
				);
				applySettings();
				
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
				m_CoordinateTransformer = std::make_unique<CoordinateTransformer>(
                    m_Window->GetSize(),
                    m_Window->GetPosition(),
                    &m_CameraPosition                    
                );

				// pause for one frame because of 
				// some weird bug with Raylib or GLFW
				pauseForOneFrame();

				m_Window->Update();
                m_CoordinateTransformer->Update();
                const CoordinateTransformer* const coordTransformer = 
                {
                    m_CoordinateTransformer.get()
                };
                m_CameraEmpty = std::make_unique<CameraEmpty>(
                    m_CoordinateTransformer.get(),
                    &c_Config,
                    &m_CameraPosition
                );
			};

        const auto initializeInput = 
            [this]()
            {
				const CoordinateTransformer* const coordTransformer = 
				{
					m_CoordinateTransformer.get()
				};
                m_InputManager = 
                {
                    std::make_unique<PlayableInputManager>(coordTransformer)
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
		initializeInput();
		initializeSound();
		initializeBackgroundMusic();
		disableEscapeKey();
		InitializeMainMenu();
		InitializeCutscene();

		m_State = State::MainMenu;
	}

	Program::~Program()
	{
		const auto writeSettingsFile = 
			[this]()
			{
				const Vector2i* positionPtr = m_Window->GetPosition();
				m_Settings->UpdateWindowPosition(*positionPtr);

				m_Settings->SaveToDefaultPath();
			};

		if (IsAudioDeviceReady())
			CloseAudioDevice();

		if (m_Font.texture.id)
			UnloadFont(m_Font);

		writeSettingsFile();
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
	}

	void Program::Update()
	{
		const auto updateControlQ =
			[this]()
			{
				if (IsKeyDown(KEY_LEFT_CONTROL) && IsKeyDown(KEY_Q))
					m_IsRunning = false;
			};

		const auto updateStateObject = 
			[this]()
			{
				switch (m_State)
				{
					case State::Game:
						m_Game->Update();
						break;

					case State::MainMenu:
						m_MainMenu->Update();
						break;

					case State::SettingsMenu:
						m_SettingsMenu->Update();
						break;

					default:
					{
						const char* const message = 
						{
							"Program::Update() failed: "
							"Invalid value of m_State enum."
						};
						throw std::runtime_error(message);
					}
				}
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

		m_CoordinateTransformer->Update();
		m_CameraEmpty->Update();
		m_Window->Update();

		if (ShouldShowCutscene())
			m_Cutscene->Update();

		updateStateObject();
		
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
		const auto drawStateObject = 
			[this]()
			{
				switch (m_State)
				{
					case State::Game:
						m_Game->Draw();
						break;

					case State::MainMenu:
						m_MainMenu->Draw();
						break;

					case State::SettingsMenu:
						m_SettingsMenu->Draw();
						break;

					default:
					{
						const char* const message = 
						{
							"Program::Draw() failed: "
							"Invalid value of m_State enum."
						};
						throw std::runtime_error(message);
					}
				}
			};

		BeginDrawing();

		const Colour4i voidColour = c_Config.GetVoidColour();
		ClearBackground(voidColour.ToRayColour());

		if (ShouldShowCutscene())
			m_Cutscene->Draw();

		drawStateObject();
		
		EndDrawing();
	}

	void Program::EndFrame()
	{
		const auto reinitializeRelevantObject = 
			[&, this](const State state)
			{
				switch (state)
				{
					case State::Game:
						InitializeGame();
						break;

					case State::MainMenu:
						InitializeMainMenu();
						break;

					case State::SettingsMenu:
						InitializeSettingsMenu();
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
				reinitializeRelevantObject(*m_PendingStateChange);
				
				m_State = *m_PendingStateChange;
				m_PendingStateChange = std::nullopt;
			};
			
		if (m_PendingStateChange)
			handleStateChange();

		if (m_ShouldCloseAfterFrame)
			m_IsRunning = false;

		if (m_GameShouldReset)
		{
			InitializeGame();
			m_GameShouldReset = false;
		}
        if (m_CutsceneShouldReset)
        {
            InitializeCutscene();
            m_CutsceneShouldReset = false;
        }

		if (WindowShouldClose())
			m_IsRunning = false;
	}

	bool Program::ShouldShowCutscene() const
	{
		return 
			(m_State == State::MainMenu) || 
			(m_State == State::SettingsMenu);
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
				m_PendingStateChange = State::SettingsMenu;
			};

		const auto playCallback =
			[this]()
			{
				m_PendingStateChange = State::Game;
			};

		const auto exitCallback = 
			[this]()
			{
				m_ShouldCloseAfterFrame = true;
			};

		m_MainMenu = std::make_unique<MainMenu>(
			&c_Config,
			m_CoordinateTransformer.get(),
			m_InputManager.get(),
			m_CameraEmpty.get(),
			&m_ImageTextureLoader,
			m_TextTextureLoader.get(),
			playCallback,
			settingsCallback,
			exitCallback,
			&m_Font,
			m_Window->GetPosition(),
			m_Window->GetSize()
		);
	}

    void Program::InitializeCutscene()
    {
        const auto resetCallback = 
            [this]()
            {
                m_CutsceneShouldReset = true;
            };

        m_Cutscene = std::make_unique<Cutscene>(
            &c_Config,
            m_CoordinateTransformer.get(),
            m_CameraEmpty.get(),
            &m_ImageTextureLoader,
            m_TextTextureLoader.get(),
            &m_SoundLoader,
            &m_CameraPosition,
            resetCallback,
            &m_Font,
            m_Window->GetPosition(),
            m_Window->GetSize()
        );       
    }

	void Program::InitializeSettingsMenu()
	{
		const auto exitCallback = 
			[this]()
			{
				m_PendingStateChange = State::MainMenu;
			};

		m_SettingsMenu = std::make_unique<SettingsMenu>(
			m_CoordinateTransformer.get(),
			m_InputManager.get(),
			&c_Config,
			&m_ImageTextureLoader,
			m_Settings.get(),
			exitCallback,
			m_CameraEmpty.get()
		);
	}
}
