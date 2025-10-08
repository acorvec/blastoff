#include "Game.h"
#include "Debug.h"
#include "GUI.h"
#include "Graphics.h"
#include "Logging.h"
#include "Player.h"
#include <memory>

namespace BlastOff
{
	Game::Game(
		const ProgramConstants* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		TextTextureLoader* const textTextureLoader,
		SoundLoader* const soundLoader,
		Vector2f* const cameraPosition,
		const Font* const font,
		const Vector2i* const windowPosition,
		const Vector2i* const windowSize
	) :
		m_ProgramConfig(programConfig),
		m_Font(font),
		m_CameraPosition(cameraPosition),
		m_ImageTextureLoader(imageTextureLoader),
		m_TextTextureLoader(textTextureLoader)
	{
		
	}

	void Game::Update()
	{
		const auto calculateCameraOffset =
			[](const float x) -> float
			{
				return -(powf(x, 1 / 3.0f) / 3);
			};

		const auto updateCameraPosition =
			[&, this]()
			{
				const Vector2f playerVelocity = m_Player->GetVelocity();
				const Vector2f pV = playerVelocity;

				float equationOffset = calculateCameraOffset(pV.y);
				if (isnan(equationOffset))
					equationOffset = calculateCameraOffset(-pV.y) * 2;

				const float extraOffset = c_Constants.GetCameraYOffset();
				const float totalOffset = equationOffset + extraOffset;

				const Rect2f playerRect = m_Player->GetEngineRect();
				const float playerY = playerRect.y;

				const float unclampedResult = playerY + totalOffset;
				const Vector2f viewportSize =
				{
					m_CoordTransformer->GetViewportSize()
				};
				const float viewportHeight = viewportSize.y;

				const float maxY =
				{
					GetWorldEdge(Direction::Up) - (viewportHeight / 2)
				};
				const float minY =
				{
					GetWorldEdge(Direction::Down) + (viewportHeight / 2)
				};

				if (unclampedResult > maxY)
					m_CameraPosition->y = maxY;
				else if (unclampedResult < minY)
					m_CameraPosition->y = minY;
				else
					m_CameraPosition->y = unclampedResult;
			};

		const auto handlePowerupCollision =
			[](Powerup* const powerup)
			{
				if (!powerup->IsCollected())
					powerup->OnCollection();
			};

		const auto updatePowerup =
			[&](Powerup* const powerup)
			{
				powerup->Update();

				const bool collision = powerup->CollideWithPlayer();
				if (collision)
					handlePowerupCollision(powerup);
			};

		const auto updatePowerups =
			[&, this]()
			{
				for (Powerup* const powerup : m_AllPowerups)
					updatePowerup(powerup);
			};

		const auto updateClouds =
			[this]()
			{
				for (Cloud* const cloud : m_AllClouds)
					cloud->Update();
			};

		const auto updateMiscObjects =
			[this]()
			{
				m_CoordTransformer->Update();
				m_InputManager->Update();

				m_Player->Update();
				m_CameraEmpty->Update();
				m_FuelBar->Update();
				m_SpeedupBar->Update();
				m_FuelBarLabel->Update();
				m_SpeedupBarLabel->Update();
			};

		const auto checkForOutcome =
			[&, this]()
			{
				if (m_Outcome != Outcome::None)
					return;

				const float playerBottom =
				{
					m_Player->GetEdgePosition(Direction::Down)
				};

				if (playerBottom > GetWorldEdge(Direction::Up))
					ChooseOutcome(Outcome::Winner);

				if (LosingConditionsAreSatisfied())
					ChooseOutcome(Outcome::Loser);
			};

#if COMPILE_CONFIG_DEBUG
		const auto checkForPlayerFreeze =
			[this]()
			{
				const int key = c_Constants.GetPlayerFreezeKey();
				const bool isFrozen = m_InputManager->GetKeyDown(key);
				m_Player->SetFrozen(isFrozen);
			};

		const auto checkForPlayerTeleport =
			[this]()
			{
				const int key = c_Constants.GetPlayerTeleportKey();

				const bool shouldTeleport = m_InputManager->GetKeyDown(key);
				if (shouldTeleport)
				{
					const float destinationY = m_WorldBounds.h * 9 / 10.0f;
					m_Player->TeleportToY(destinationY);
				}
			};

		const auto checkForEmptyingPlayerFuel =
			[this]()
			{
				const int key = c_Constants.GetEmptyPlayerFuelKey();
				const bool shouldEmptyFuel = IsKeyDown(key);
				if (shouldEmptyFuel)
					m_Player->EmptyFuel();
			};

		const auto updateDebugTools =
			[&]()
			{
				checkForPlayerFreeze();
				checkForPlayerTeleport();
				checkForEmptyingPlayerFuel();
			};
#endif

		updateCameraPosition();
		updatePowerups();
		updateClouds();
		checkForOutcome();
		updateMiscObjects();

#if COMPILE_CONFIG_DEBUG
		if (m_ProgramConfig->GetDebugToolsEnabled())
			updateDebugTools();
#endif
	}

	void Game::Draw() const
	{
		const auto drawCloud =
			[this](const Cloud* const cloud, const bool isDrawingAfterPlayer)
			{
				const bool drawsAbovePlayer = cloud->DrawsAbovePlayer();
				if (drawsAbovePlayer == isDrawingAfterPlayer)
					cloud->Draw();
			};

		const auto drawPowerups =
			[this]()
			{
				for (const Powerup* const powerup : m_AllPowerups)
					powerup->Draw();
			};

		const auto drawCloudsBelowPlayer =
			[&, this]()
			{
				for (const Cloud* const cloud : m_AllClouds)
					drawCloud(cloud, false);
			};

		const auto drawCloudsAbovePlayer =
			[&, this]()
			{
				for (const Cloud* const cloud : m_AllClouds)
					drawCloud(cloud, true);
			};

		const auto drawObjects =
			[&, this]()
			{
				m_Background->Draw();
				m_Crag->Draw();
				m_Platform->Draw();

				drawCloudsBelowPlayer();
				drawPowerups();
				m_Player->Draw();
				drawCloudsAbovePlayer();

				m_FuelBar->Draw();
				m_SpeedupBar->Draw();
				m_FuelBarLabel->Draw();
				m_SpeedupBarLabel->Draw();
			};

		drawObjects();
	}

	void Game::ChooseOutcome(const Outcome outcome)
	{
		m_Outcome = outcome;
	}

	void Game::FinishConstruction(
		CoordinateTransformer* const coordTransformer,
		CameraEmpty* const cameraEmpty,
		unique_ptr<InputManager> inputManager
	)
	{
		const auto initializeBackgroundSprite =
			[&, this]()
			{
				const Vector2f viewportSize =
				{
					m_CoordTransformer->GetViewportSize()
				};
				const float worldBoundHeight =
				{
					c_Constants.GetWorldBoundHeight()
				};
				const float backgroundY =
				{
					(worldBoundHeight - viewportSize.y) / 2.0f
				};

				m_WorldBounds =
				{
					0, backgroundY,
					viewportSize.x, worldBoundHeight
				};
				m_Background = std::make_unique<Background>(
					&m_WorldBounds,
					m_CoordTransformer,
					m_ProgramConfig
				);
			};

		const auto initializeCrag =
			[&, this]()
			{
				m_Crag = std::make_unique<Crag>(
					m_CoordTransformer,
					m_ProgramConfig,
					m_ImageTextureLoader
				);
			};

		const auto initializePlatform =
			[&, this]()
			{
				const float platformHeight = c_Constants.GetPlatformHeight();
				m_Platform = std::make_unique<Platform>(
					platformHeight,
					m_CoordTransformer,
					m_ProgramConfig,
					m_ImageTextureLoader
				);
			};

		const auto initializeCloudDirection =
			[this]()
			{
				const float x = GetRandomFloat();
				if (x > 0.5f)
					m_CloudMovementDirection = Direction::Right;
				else
					m_CloudMovementDirection = Direction::Left;
			};

		const auto initializeCloud =
			[&, this](auto& vector)
			{
				vector.emplace_back(
					m_CoordTransformer,
					m_ProgramConfig,
					m_ImageTextureLoader,
					&m_WorldBounds,
					&m_CloudMovementDirection
				);
			};

		const auto initializeLowClouds =
			[&, this]()
			{
				const size_t length = c_Constants.GetAmountOfLowClouds();
				m_LowClouds.reserve(length);

				for (size_t index = 0; index < length; index++)
					initializeCloud(m_LowClouds);
			};

		const auto initializeHighClouds =
			[&, this]()
			{
				const size_t length = c_Constants.GetAmountOfLowClouds();
				m_HighClouds.reserve(length);

				for (size_t index = 0; index < length; index++)
					initializeCloud(m_HighClouds);
			};

		const auto initializeAllCloudsVector =
			[this]()
			{
				const size_t totalLength =
				{
					m_LowClouds.size() + m_HighClouds.size()
				};
				m_AllClouds.reserve(totalLength);

				for (Cloud& cloud : m_LowClouds)
					m_AllClouds.push_back(&cloud);
				for (Cloud& cloud : m_HighClouds)
					m_AllClouds.push_back(&cloud);
			};

		const auto initializeClouds =
			[&]()
			{
				initializeCloudDirection();

				initializeLowClouds();
				initializeHighClouds();

				initializeAllCloudsVector();	
			};

		const auto initializePlayer =
			[&, this]()
			{
				m_Player = std::make_unique<Player>(
					&m_Outcome,
					&m_WorldBounds,
					m_Platform.get(),
					m_CoordTransformer,
					&c_Constants,
					m_ProgramConfig,
					m_InputManager.get(),
					m_ImageTextureLoader
				);
			};

		const auto updatePlatformCollisionRect =
			[this]()
			{
				const Rect2f playerRect = m_Player->GetEngineRect();
				const Vector2f playerSize = playerRect.GetSize();

				m_Platform->UpdateCollisionRect(playerSize);
			};

		const auto calculateRandomOffset =
			[this](const float randomYRange) -> Vector2f
			{
				return
				{
					(GetRandomFloat() - 0.5f) * m_WorldBounds.w,
					GetRandomFloat() * randomYRange
				};
			};

		const auto initializePowerup =
			[&](
				const size_t index,
				const size_t length,
				auto& vector
			)
			{
				constexpr float randomness = 2;

				const float marginMultiplier = 19 / 20.0f;
				const float multiplier = marginMultiplier;

				const float divisor = ((float)length) + randomness;
				const float stride =
				{
					(m_WorldBounds.h * multiplier) / divisor
				};
				const float randomYRange =
				{
					stride * marginMultiplier * randomness
				};
				const Vector2f bottomOfRange =
				{
					0,
					(index + 1) * (float)stride
				};

				const Vector2f randomOffset =
				{
					calculateRandomOffset(randomYRange)
				};
				const Vector2f enginePosition = bottomOfRange + randomOffset;
#if COMPILE_CONFIG_DEBUG
				const float worldTop = GetWorldEdge(Direction::Up);
				const float worldBottom = GetWorldEdge(Direction::Down);

				if ((bottomOfRange.y + randomYRange) > worldTop)
				{
					Logging::Log("Powerup random range goes too high.");
					BreakProgram();
				}
				if (bottomOfRange.y < worldBottom)
				{
					Logging::Log("Powerup random range goes too low.");
					BreakProgram();
				}
#endif
				vector.emplace_back(
					m_CoordTransformer,
					m_ProgramConfig,
					m_Player.get(),
					m_ImageTextureLoader,
					enginePosition
				);
			};

		const auto initializePowerupVector =
			[&](const size_t length, auto& vector)
			{
				vector.reserve(length);

				for (size_t index = 0; index < length; index++)
					initializePowerup(index, length, vector);
			};

		const auto initializeSpeedupPowerups =
			[&]()
			{
				const size_t length =
				{
					c_Constants.GetAmountOfSpeedPowerups()
				};
				initializePowerupVector(length, m_SpeedUpPowerups);
			};

		const auto initializeFuelUpPowerups =
			[&]()
			{
				const size_t length =
				{
					c_Constants.GetAmountOfFuelPowerups()
				};
				initializePowerupVector(length, m_FuelUpPowerups);
			};

		const auto initializeAllPowerupsVector =
			[this]()
			{
				const size_t totalLength =
				{
					m_SpeedUpPowerups.size() + m_FuelUpPowerups.size()
				};
				m_AllPowerups.reserve(totalLength);

				for (Powerup& powerup : m_SpeedUpPowerups)
					m_AllPowerups.push_back(&powerup);
				for (Powerup& powerup : m_FuelUpPowerups)
					m_AllPowerups.push_back(&powerup);
			};

		const auto initializePowerups =
			[&]()
			{
				initializeSpeedupPowerups();
				initializeFuelUpPowerups();

				initializeAllPowerupsVector();
			};

		const auto initializeGUIBars =
			[&, this]()
			{
				m_FuelBar = std::make_unique<FuelBar>(
					m_CoordTransformer,
					m_ProgramConfig,
					m_ImageTextureLoader,
					m_CameraEmpty,
					m_Player.get()
				);
				m_SpeedupBar = std::make_unique<SpeedupBar>(
					m_CoordTransformer,
					m_ProgramConfig,
					m_ImageTextureLoader,
					m_CameraEmpty,
					m_Player.get()
				);
			};

		const auto initializeGUILabels =
			[&, this]()
			{
				m_FuelBarLabel = std::make_unique<FuelBarLabel>(
					m_FuelBar.get(),
					m_CoordTransformer,
					m_ProgramConfig,
					m_TextTextureLoader,
					m_Font
				);
				m_SpeedupBarLabel = std::make_unique<SpeedupBarLabel>(
					m_SpeedupBar.get(),
					m_CoordTransformer,
					m_ProgramConfig,
					m_TextTextureLoader,
					m_Font
				);
			};

		const auto initializeObjects =
			[&]()
			{
				initializeBackgroundSprite();
				initializeCrag();
				initializePlatform();
				initializeClouds();
				initializePlayer();
				initializePowerups();
				initializeGUIBars();
				initializeGUILabels();
			};

		m_CoordTransformer = coordTransformer;
		m_CameraEmpty = cameraEmpty;
		m_InputManager = std::move(inputManager);

		initializeObjects();
		updatePlatformCollisionRect();
	}

	float Game::GetWorldEdge(const Direction side) const
	{
		const optional<float> value =
		{
			m_WorldBounds.GetEdgePosition(side)
		};
		if (!value)
		{
			const string message =
			{
				"Rect2f::GetEdgePosition"
				"(" + DirectionToString(side) + ") failed."
			};
			throw std::runtime_error(message);
		}
		return *value;
	}

	bool Game::LosingConditionsAreSatisfied() const
	{
		const float playerTop = m_Player->GetEdgePosition(Direction::Up);
		if (playerTop < GetWorldEdge(Direction::Down))
			return true;

		// losing condition:
		// if the player stays stationary for too long without fuel OR
		// while the outcome is chosen
		const bool fuelIsEmpty = m_Player->IsOutOfFuel();
		if (m_Player->IsStationary() && fuelIsEmpty)
			return true;

		const bool outcomeIsChosen = (m_Outcome != Outcome::None);
		if (m_Player->IsStationary() && outcomeIsChosen)
			return true;

		return false;
	}


	PlayableGame::PlayableGame(
        const bool* const programIsMuted,
        const ProgramConstants* const programConfig,
        ImageTextureLoader* const imageTextureLoader,
        TextTextureLoader* const textTextureLoader,
        SoundLoader* const soundLoader,
        const Callback& muteUnmuteCallback,
        const Callback& resetCallback,
		const Callback& exitCallback,
        const Font* const font,
        const Vector2i* const windowPosition,
        const Vector2i* const windowSize
    ) :
        Game(
            programConfig,
            imageTextureLoader,
            textTextureLoader,
            soundLoader,
			&m_CameraPosition,
            font,
            windowPosition,
            windowSize
        )
    {
		const auto initializeGraphics =
			[&, this]()
			{
				m_CoordinateTransformer = std::make_unique<CoordinateTransformer>(
					windowSize,
					windowPosition,
					&m_CameraPosition
				);
                m_CoordinateTransformer->Update();
				m_CameraEmpty = std::make_unique<CameraEmpty>(
					m_CoordinateTransformer.get(),
					m_ProgramConfig,
					&m_CameraPosition
				);
			};

        const auto initializeSound =
            [&, this]()
        {
            const bool isSoundEnabled = m_ProgramConfig->GetSoundEnabled();
            if (!isSoundEnabled)
                return;

            m_WinSound = soundLoader->LazyLoadSound("win.wav");
            m_LoseSound = soundLoader->LazyLoadSound("lose.wav");
            m_EasterEggSound1 = soundLoader->LazyLoadSound("egg1.wav");
            m_EasterEggSound2 = soundLoader->LazyLoadSound("egg2.wav");
        };

        const auto initializeInput =
            [&, this]()
            {
                const auto coordTransformer = m_CoordinateTransformer.get();
                unique_ptr<InputManager> inputManager =
                {
                    std::make_unique<PlayableInputManager>(coordTransformer)
                };
                FinishConstruction(
					m_CoordinateTransformer.get(),
					m_CameraEmpty.get(),
					std::move(inputManager)
				);
            };

		const auto initializeGameEndMenus =
			[&, this]()
			{
				m_WinMenu = std::make_unique<WinMenu>(
					resetCallback,
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
					m_ProgramConfig,
					imageTextureLoader,
					textTextureLoader,
					m_CameraEmpty.get(),
					font
				);
				m_LoseMenu = std::make_unique<LoseMenu>(
					resetCallback,
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
					m_ProgramConfig,
					imageTextureLoader,
					textTextureLoader,
					m_CameraEmpty.get(),
					font
				);
			};

		const auto initializeGUIButtons =
			[&, this]()
			{
                m_MuteButton = std::make_unique<MuteButton>(
					programIsMuted,
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
                    m_ProgramConfig,
                    imageTextureLoader,
                    muteUnmuteCallback,
                    m_CameraEmpty.get()
                );
				m_ResetButton = std::make_unique<TopRightResetButton>(
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
					m_ProgramConfig,
					imageTextureLoader,
					resetCallback,
					m_CameraEmpty.get()
				);
				m_ExitButton = std::make_unique<TopRightExitButton>(
					m_CoordinateTransformer.get(),
					m_InputManager.get(),
					m_ProgramConfig,
					imageTextureLoader,
					exitCallback,
					m_CameraEmpty.get()
				);
			};

		initializeGraphics();
        initializeSound();
        initializeInput();
        initializeGameEndMenus();
        initializeGUIButtons();
    }

    void PlayableGame::Update()
    {
        const auto updateMiscObjects =
            [this]()
            {
                m_WinMenu->Update();
                m_LoseMenu->Update();
                m_MuteButton->Update();
                m_ResetButton->Update();
				m_ExitButton->Update();
            };

        Game::Update();

        updateMiscObjects();
    }

    void PlayableGame::Draw() const
    {
        Game::Draw();

        // TODO: if the PlayableGame class ever needs to draw something
        // below ANYTHING in the Game class, this will literally not be
        // possible.
        // a fix may be needed eventually
        m_WinMenu->Draw();
        m_LoseMenu->Draw();
        m_MuteButton->Draw();
        m_ResetButton->Draw();
		m_ExitButton->Draw();
    }

    void PlayableGame::ChooseOutcome(const Outcome outcome)
	{
		const auto playOutcomeSound =
			[this]()
			{
				const bool isSoundEnabled = m_ProgramConfig->GetSoundEnabled();
				if (!isSoundEnabled)
					return;

				const float easterEggTest = GetRandomFloat();

				if (easterEggTest < powf(10, -3))
					PlaySound(*m_EasterEggSound2);
				else if (easterEggTest < powf(10, -2))
					PlaySound(*m_EasterEggSound1);

				else if (m_Outcome == Outcome::Winner)
					PlaySound(*m_WinSound);
				else if (m_Outcome == Outcome::Loser)
					PlaySound(*m_LoseSound);
				else
				{
					throw std::runtime_error(
						"Game::Update(): "
						"Unable to determine which Sound to play "
						"after getting the GameOutcome. "
					);
				}
			};

		const auto getRelevantEndMenu =
			[this]() -> EndMenu*
			{
				switch (m_Outcome)
				{
					case Outcome::Winner:
						return m_WinMenu.get();

					case Outcome::Loser:
						return m_LoseMenu.get();

					case Outcome::None:
					default:
						return nullptr;
				}
			};

		Game::ChooseOutcome(outcome);
		playOutcomeSound();

		EndMenu* const endMenu = getRelevantEndMenu();
		if (endMenu)
			endMenu->Enable();

		m_ResetButton->SlideOut();
	}


    Cutscene::Cutscene(
		const ProgramConstants* const programConfig,
		CoordinateTransformer* const coordTransformer,
		CameraEmpty* const cameraEmpty,
		ImageTextureLoader* const imageTextureLoader,
		TextTextureLoader* const textTextureLoader,
		SoundLoader* const soundLoader,
		Vector2f* const cameraPosition,
		const Callback& resetCallback,
		const Font* const font,
		const Vector2i* const windowPosition,
		const Vector2i* const windowSize
	) :
		Game(
			programConfig,
			imageTextureLoader,
			textTextureLoader,
			soundLoader,
			cameraPosition,
			font,
			windowPosition,
			windowSize
		),
		m_ResetCallback(resetCallback)
	{
		unique_ptr<InputManager> inputManager =
		{
			std::make_unique<CutsceneInputManager>(
				coordTransformer,
				cameraPosition,
				m_ProgramConfig
			)
		};
		FinishConstruction(
			coordTransformer,
			cameraEmpty,
			std::move(inputManager)
		);
	}

	void Cutscene::Update()
	{
		const auto updateResetTimer =
			[this]()
			{
				m_ResetTick -= m_ProgramConfig->GetTargetFrametime();
				if (!ResetTimerIsActive())
					m_ResetCallback();
			};

		const auto checkForReset =
			[this]()
			{
				const bool alreadyResetting = ResetTimerIsActive();
				const bool shouldReset = LosingConditionsAreSatisfied();

				if (LosingConditionsAreSatisfied())
					m_ResetTick = c_MaxResetTick;
			};

		Game::Update();

		if (ResetTimerIsActive())
			updateResetTimer();
		else
			checkForReset();
	}

	const float Cutscene::c_MaxResetTick = 1;

	bool Cutscene::ResetTimerIsActive() const
	{
		return m_ResetTick >= 0;
	}
}
