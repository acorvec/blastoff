#include "Props.h"

#if COMPILE_CONFIG_DEBUG
#include "Logging.h"
#endif

namespace BlastOff
{
	Crag::Crag(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader
	)
	{
		m_Sprite = ImageSprite::LoadFromPath(
			c_TexturePath,
			coordTransformer,
			programConfig,
			imageTextureLoader
		);

		const Vector2i imageSize = m_Sprite->GetImageSize();
		const Vector2f viewportSize = coordTransformer->GetViewportSize();

		const float engineWidth = viewportSize.x * 2;
		const Vector2f engineSize =
		{
			engineWidth,
			engineWidth * imageSize.y / (float)imageSize.x
		};
		const Vector2f enginePosition = { 0, -engineSize.y };

		const Rect2f engineRect(enginePosition, engineSize);
		m_Sprite->SetEngineRect(engineRect);
	}

	void Crag::Draw() const
	{
		m_Sprite->Draw();
	}

	const char* const Crag::c_TexturePath = "Crag.png";


	Platform::Platform(
		const float platformHeight,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader
	)
	{
		m_Sprite = ImageSprite::LoadFromPath(
			c_TexturePath,
			coordTransformer,
			programConfig,
			imageTextureLoader
		);

		const Vector2f viewportSize = coordTransformer->GetViewportSize();
		const float engineY = (platformHeight - viewportSize.y) / 2.0f;
		const Vector2f enginePosition = { 0, engineY };

		const Vector2i imageSize = m_Sprite->GetImageSize();
		const Vector2f engineSize =
		{
			platformHeight * imageSize.x / (float)imageSize.y,
			platformHeight
		};

		const Rect2f engineRect(enginePosition, engineSize);
		m_Sprite->SetEngineRect(engineRect);
	}

	void Platform::Draw() const
	{
		m_Sprite->Draw();
	}

	void Platform::UpdateCollisionRect(const Vector2f playerSize)
	{
		const Rect2f engineRect = m_Sprite->GetEngineRect();
		const Vector2f engineSize = engineRect.GetSize();

		const Rect2f collisionRect(
			engineRect.GetPosition(),
			{ engineSize.x - (playerSize.x / 2.0f), engineSize.y }
		);
		m_Sprite->SetCollisionRect(collisionRect);
	}

	Vector2f Platform::GetEngineSize() const
	{	
		return m_Sprite->GetEngineSize();
	}

	Line2f Platform::GetTopCollisionLine() const
	{
		return m_Sprite->GetTopCollisionLine();
	}

	Line2f Platform::GetLeftCollisionLine() const
	{
		return m_Sprite->GetLeftCollisionLine();
	}

	Line2f Platform::GetRightCollisionLine() const
	{
		return m_Sprite->GetRightCollisionLine();
	}

	const char* const Platform::c_TexturePath = "Platform.png";


	BackgroundConfiguration::BackgroundConfiguration() :
		m_LowerAtmosphereColour(0xA8, 0xFF, 0xFF),
		m_MiddleAtmosphereColour(0x32, 0x5B, 0xFF),
		m_UpperAtmosphereColour(0x0C, 0x10, 0x21),
		m_LowerAtmosphereSpriteTop(0.67578125f)
	{

	}

	Colour4i BackgroundConfiguration::GetLowerAtmosphereColour() const
	{
		return m_LowerAtmosphereColour;
	}

	Colour4i BackgroundConfiguration::GetMiddleAtmosphereColour() const
	{
		return m_MiddleAtmosphereColour;
	}

	Colour4i BackgroundConfiguration::GetUpperAtmosphereColour() const
	{
		return m_UpperAtmosphereColour;
	}

	float BackgroundConfiguration::GetLowerAtmosphereSpriteTop() const
	{
		return m_LowerAtmosphereSpriteTop;
	}


	Background::Background(
		const Rect2f* const worldBounds,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig
	) :
		m_WorldBounds(worldBounds),
		m_CoordTransformer(coordTransformer),
		m_ProgramConfig(programConfig),
		m_MostRecentWorldBounds(*worldBounds)
	{
		InitializeSprites();
	}

	void Background::Update()
	{
		const auto checkForWorldBoundsChange =
			[this]()
			{
				if ((*m_WorldBounds) != m_MostRecentWorldBounds)
					InitializeSprites();
			};

		checkForWorldBoundsChange();
	}

	void Background::Draw() const
	{
		m_LowerAtmosphereSprite->Draw();
		m_UpperAtmosphereSprite->Draw();
	}

	void Background::InitializeSprites()
	{
#if COMPILE_CONFIG_DEBUG
		if (m_SpriteInitializationCount > 0)
		{
			const char* const message =
			{
				"initializing background sprites due to a "
				"change in world size. this should not be done "
				"more than once (after initializing the background)."
			};
			Logging::Log(message);
			BreakProgram();
		}
		m_SpriteInitializationCount++;
#endif
		m_MostRecentWorldBounds = *m_WorldBounds;

		InitializeLowerAtmosphereSprite();
		InitializeUpperAtmosphereSprite();
	}

	void Background::InitializeLowerAtmosphereSprite()
	{
		const Vector2f viewportSize = m_CoordTransformer->GetViewportSize();
		const Vector2f viewportOffset = { 0, -viewportSize.y / 2.0f };

		const Vector2f worldPosition = m_WorldBounds->GetPosition();
		const Vector2f worldSize = m_WorldBounds->GetSize();

		const float joinPoint = c_Config.GetLowerAtmosphereSpriteTop();
		const Vector2f sizeFactor = { 1, joinPoint };
		const Vector2f engineSize = worldSize * sizeFactor;
		const Vector2f top = { 0, worldSize.y * joinPoint };

		const Vector2f sizeOffset = { 0, engineSize.y / 2.0f };
		const Vector2f enginePosition = top - sizeOffset + viewportOffset;

		const Rect2f engineRect(enginePosition, engineSize);
		m_LowerAtmosphereSprite = std::make_unique<GradientSprite>(
			engineRect,
			m_CoordTransformer,
			m_ProgramConfig,
			c_Config.GetLowerAtmosphereColour(),
			c_Config.GetMiddleAtmosphereColour(),
			Direction::Up
		);
	}

	void Background::InitializeUpperAtmosphereSprite()
	{
		const Vector2f viewportSize = m_CoordTransformer->GetViewportSize();
		const Vector2f viewportOffset = { 0, -viewportSize.y / 2.0f };

		const Vector2f worldPosition = m_WorldBounds->GetPosition();
		const Vector2f worldSize = m_WorldBounds->GetSize();

		const float joinPoint = c_Config.GetLowerAtmosphereSpriteTop();
		const Vector2f sizeFactor = { 1, 1 - joinPoint };
		const Vector2f engineSize = worldSize * sizeFactor;
		const Vector2f bottom = { 0, worldSize.y * joinPoint };

		const Vector2f sizeOffset = { 0, engineSize.y / 2.0f };
		const Vector2f enginePosition =
		{
			bottom + sizeOffset + viewportOffset
		};

		const Rect2f engineRect(enginePosition, engineSize);
		m_UpperAtmosphereSprite = std::make_unique<GradientSprite>(
			engineRect,
			m_CoordTransformer,
			m_ProgramConfig,
			c_Config.GetMiddleAtmosphereColour(),
			c_Config.GetUpperAtmosphereColour(),
			Direction::Up
		);
	}


	void Cloud::Update()
	{
		const auto move =
			[this]()
			{
				const float targetFrametime = 
				{
					m_ProgramConfig->GetTargetFrametime()
				};
				const float xMovement = CalculateSpeed() * targetFrametime;
				const Vector2f translation = { xMovement, 0 };

				m_Sprite->Move(translation);
			};

		const auto calculateMovementDirection =
			[this]() -> Direction
			{
				if (CalculateSpeed() > 0)
					return Direction::Right;
				else
					return Direction::Left;
			};

		const auto reinitializePosition =
			[this]()
			{
				InitializeYPosition();

				const float maxTranslation = CalculateMaxInitXTranslation();
				float xTranslation = maxTranslation;

				const bool isRightMovement =
				{
					(*m_MovementDirection) == Direction::Right
				};
				if (isRightMovement)
					xTranslation *= -1;

				m_Sprite->Move({ xTranslation, 0 });
			};

		const auto checkForRespawn =
			[&, this]()
			{
				const Direction direction = calculateMovementDirection();
				const Rect2f engineRect = m_Sprite->GetEngineRect();

				const optional<float> edge =
				{
					engineRect.GetOppositeEdgePosition(direction)
				};
				if (!edge)
				{
					const string message =
					{
						"Rect2f::GetOppositeEdgePosition"
						"(" + DirectionToString(direction) + ") failed."
					};
					throw std::runtime_error(message);
				}

				const optional<float> worldEdge =
				{
					m_WorldBounds->GetEdgePosition(direction)
				};
				if (!worldEdge)
				{
					const string message =
					{
						"Rect2f::GetEdgePosition"
						"(" + DirectionToString(direction) + ") failed."
					};
					throw std::runtime_error(message);
				}

				const bool isLeftMovement = (direction == Direction::Left);
				const bool isRightMovement = (direction == Direction::Right);

				const bool isGreater = ((*edge) < (*worldEdge));
				const bool isLesser = ((*edge) > (*worldEdge));

				if (isLeftMovement && isGreater)
					reinitializePosition();
				else if (isRightMovement && isLesser)
					reinitializePosition();
			};

		move();
		checkForRespawn();
	}

	void Cloud::Draw() const
	{
		m_Sprite->Draw();
	}

	bool Cloud::DrawsAbovePlayer() const
	{
		return m_DrawsAbovePlayer;
	}

	Cloud::Cloud(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const Rect2f* const worldBounds,
		const Direction* const movementDirection,
		const char* const texturePath,
		const SpawningRange spawningRange,
		const float speedMultiplier,
		const float speedRandomness,
		const Vector2f engineSize
	) :
		m_ProgramConfig(programConfig),
		m_WorldBounds(worldBounds),
		m_MovementDirection(movementDirection),
		m_SpawningRange(spawningRange),
		m_SpeedMultiplier(speedMultiplier),
		m_SpeedRandomness(speedRandomness),
		m_EngineSize(engineSize)
	{
		const auto initializePosition =
			[this]()
			{
				InitializeYPosition();

				const float maxTranslation = CalculateMaxInitXTranslation();
				const float multiplier = GetRandomFloat() - 0.5f;
				const float xTranslation = maxTranslation * multiplier;

				m_Sprite->Move({ xTranslation, 0 });
			};

		const auto initializeSprite =
			[&, this]()
			{
				const Texture* const texture =
				{
					imageTextureLoader->LazyLoadTexture(texturePath)
				};
				m_Sprite = std::make_unique<ImageSprite>(
					coordTransformer,
					programConfig,
					texture
				);
			};

		const auto calculateRandomSpeedMultiplier =
			[this]()
			{
				const float x = GetRandomFloat() - 0.5f;
				m_RandomSpeedMultiplier = 1 + (x * m_SpeedRandomness);
			};

		const auto initializeDrawsAbovePlayer =
			[this]()
			{
				m_DrawsAbovePlayer = GetRandomFloat() > 0.5f;
			};

		initializeSprite();
		initializePosition();
		calculateRandomSpeedMultiplier();
		initializeDrawsAbovePlayer();
	}

	float Cloud::CalculateSpeed() const
	{
		const float unsignedResult = 
		{
			m_SpeedMultiplier* m_RandomSpeedMultiplier
		};
		const bool isLeftMovement = 
		{
			(*m_MovementDirection) == Direction::Left
		};

		if (isLeftMovement)
			return -unsignedResult;
		else
			return unsignedResult;
	}

	void Cloud::InitializeYPosition()
	{
		const float x = GetRandomFloat();

		const float start = m_SpawningRange.bottom;
		const float end = m_SpawningRange.top;

		const float yPosition = start + (x * (end - start));
		const Vector2f enginePosition = { 0, yPosition };
		const Rect2f engineRect(enginePosition, m_EngineSize);

		m_Sprite->SetEngineRect(engineRect);
	}

	float Cloud::CalculateMaxInitXTranslation() const
	{
		const Rect2f engineRect = m_Sprite->GetEngineRect();
		return engineRect.w + m_WorldBounds->w;
	}


	LowCloud::LowCloud(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const Rect2f* const worldBounds,
		const Direction* const movementDirection
	) :
		Cloud(
			coordTransformer,
			programConfig,
			imageTextureLoader,
			worldBounds,
			movementDirection,
			c_TexturePath,
			c_SpawningRange,
			c_SpeedMultiplier,
			c_SpeedRandomness,
			c_EngineSize
		)
	{
		
	}

	const char* const LowCloud::c_TexturePath = "LowCloud.png";
	const CloudSpawningRange LowCloud::c_SpawningRange = 
	{
		5.4863021f, 
		261.4863f 
	};
	const float LowCloud::c_SpeedMultiplier = 1;
	const float LowCloud::c_SpeedRandomness = 2 / 10.0f;
	const Vector2f LowCloud::c_EngineSize = { 2, 1 };


	HighCloud::HighCloud(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const Rect2f* const worldBounds,
		const Direction* const movementDirection
	) :
		Cloud(
			coordTransformer,
			programConfig,
			imageTextureLoader,
			worldBounds,
			movementDirection,
			c_TexturePath,
			c_SpawningRange,
			c_SpeedMultiplier,
			c_SpeedRandomness,
			c_EngineSize
		)
	{

	}

	const char* const HighCloud::c_TexturePath = "HighCloud.png";

	const CloudSpawningRange HighCloud::c_SpawningRange =
	{
		256,
		409.6f
	};
	const float HighCloud::c_SpeedMultiplier = 3 / 2.0f;
	const float HighCloud::c_SpeedRandomness = 2 / 10.0f;
	const Vector2f HighCloud::c_EngineSize =
	{
		7.556392017f * 2 / 3.0f, 2 / 3.0f
	};
}
