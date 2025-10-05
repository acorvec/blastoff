#include "Game.h"
#include "Debug.h"
#include "Logging.h"
#include "Player.h"

namespace BlastOff
{
	Game::Game(
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		TextTextureLoader* const textTextureLoader,
		SoundLoader* const soundLoader,
		const Font* const font,
		const Vector2i* const windowPosition,
		const Vector2i* const windowSize
	) :
		m_ProgramConfig(programConfig),
		m_Font(font),
		m_ImageTextureLoader(imageTextureLoader),
		m_TextTextureLoader(textTextureLoader)
	{
		const auto initializeGraphics =
			[&, this]()
			{
				using CoordTransformer = CoordinateTransformer;
				m_CoordinateTransformer = std::make_unique<CoordTransformer>(
					windowSize,
					windowPosition,
					&m_CameraPosition
				);
				m_CoordinateTransformer->Update();
			};

		const auto initializeCameraEmpty =
			[&, this]()
			{
				const Rect2f engineRect(
					m_CameraPosition,
					Vector2f::Zero()
				);
				m_CameraEmpty = std::make_unique<CameraEmpty>(
					m_CoordinateTransformer.get(), 
					m_ProgramConfig,
					&m_CameraPosition
				);
			};

		const auto initializeBackgroundSprite =
			[&, this]()
			{
				const Vector2f viewportSize =
				{
					m_CoordinateTransformer->GetViewportSize()
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
					m_CoordinateTransformer.get(),
					m_ProgramConfig
				);
			};

		const auto initializeCrag =
			[&, this]()
			{
				m_Crag = std::make_unique<Crag>(
					m_CoordinateTransformer.get(),
					m_ProgramConfig,
					imageTextureLoader
				);
			};

		const auto initializePlatform =
			[&, this]()
			{
				const float platformHeight = c_Constants.GetPlatformHeight();
				m_Platform = std::make_unique<Platform>(
					platformHeight,
					m_CoordinateTransformer.get(),
					m_ProgramConfig,
					imageTextureLoader
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
					m_CoordinateTransformer.get(),
					m_ProgramConfig,
					imageTextureLoader,
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

		const auto initializeObjects =
			[&]()
			{
				initializeCameraEmpty();
				initializeBackgroundSprite();
				initializeCrag();
				initializePlatform();
				initializeClouds();
			};

		initializeGraphics();
		initializeObjects();
	}

	void Game::FinishConstruction(unique_ptr<InputManager> inputManager)
	{
		const auto initializePlayer =
			[&, this]()
			{
				m_Player = std::make_unique<Player>(
					&m_Outcome,
					&m_WorldBounds,
					m_Platform.get(),
					m_CoordinateTransformer.get(),
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
					m_CoordinateTransformer.get(),
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
					m_CoordinateTransformer.get(),
					m_ProgramConfig,
					m_ImageTextureLoader,
					m_CameraEmpty.get(),
					m_Player.get()
				);
				m_SpeedupBar = std::make_unique<SpeedupBar>(
					m_CoordinateTransformer.get(),
					m_ProgramConfig,
					m_ImageTextureLoader,
					m_CameraEmpty.get(),
					m_Player.get()
				);
			};

		const auto initializeGUILabels =
			[&, this]()
			{
				m_FuelBarLabel = std::make_unique<FuelBarLabel>(
					m_FuelBar.get(),
					m_CoordinateTransformer.get(),
					m_ProgramConfig,
					m_TextTextureLoader,
					m_Font
				);
				m_SpeedupBarLabel = std::make_unique<SpeedupBarLabel>(
					m_SpeedupBar.get(),
					m_CoordinateTransformer.get(),
					m_ProgramConfig,
					m_TextTextureLoader,
					m_Font
				);
			};

		m_InputManager = std::move(inputManager);

		initializePlayer();
		initializePowerups();
		initializeGUIBars();
		initializeGUILabels();
		updatePlatformCollisionRect();
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
					m_CoordinateTransformer->GetViewportSize()
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
					m_CameraPosition.y = maxY;
				else if (unclampedResult < minY)
					m_CameraPosition.y = minY;
				else
					m_CameraPosition.y = unclampedResult;
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
				m_CoordinateTransformer->Update();
				m_InputManager->Update();

				m_Player->Update();
				m_CameraEmpty->Update();
				m_FuelBar->Update();
				m_SpeedupBar->Update();
				m_FuelBarLabel->Update();
				m_SpeedupBarLabel->Update();
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
}
