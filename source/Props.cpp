#include "Props.h"
#include "Graphics.h"
#include "OperatingSystem.h"
#include "Logging.h"
#include "Utils.h"
#include "raylib.h"
#include <memory>

namespace BlastOff
{
	float SpawningRange::ChooseYPosition() const
	{
		const float num = GetRandomFloat();

		const float start = bottom;
		const float end = top;

		const float result = start + (num * (end - start));
		return result;
	}


	Crag::Crag(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader
	)
	{
		m_Sprite = ImageSprite::LoadFromPath(
			c_TexturePath,
			coordTransformer,
			programConstants,
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

	const char* const Crag::c_TexturePath = "props/crag.png";


	SpawnPlatform::SpawnPlatform(
		const float platformHeight,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader
	)
	{
		m_Sprite = ImageSprite::LoadFromPath(
			c_TexturePath,
			coordTransformer,
			programConstants,
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

	void SpawnPlatform::UpdateCollisionRect(const Vector2f playerSize)
	{
		const Rect2f engineRect = m_Sprite->GetEngineRect();
		const Vector2f engineSize = engineRect.GetSize();

		const Rect2f collisionRect(
			engineRect.GetPosition(),
			{ engineSize.x - (playerSize.x / 2.0f), engineSize.y }
		);
		m_Sprite->SetCollisionRect(collisionRect);
	}

	Vector2f SpawnPlatform::GetEngineSize() const
	{	
		return m_Sprite->GetEngineSize();
	}

	Line2f SpawnPlatform::GetTopCollisionLine() const
	{
		return m_Sprite->GetTopCollisionLine();
	}

	Line2f SpawnPlatform::GetLeftCollisionLine() const
	{
		return m_Sprite->GetLeftCollisionLine();
	}

	Line2f SpawnPlatform::GetRightCollisionLine() const
	{
		return m_Sprite->GetRightCollisionLine();
	}

	void SpawnPlatform::Draw() const
	{
		m_Sprite->Draw();
	}

	const char* const SpawnPlatform::c_TexturePath = "props/spawnPlatform.png";


	FloatingPlatformSegment::FloatingPlatformSegment(
		const Sprite* const parent,
		const size_t segmentIndex,
		const size_t amountOfSegments,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader
	)
	{
		const auto calculateEngineRect =
			[&, this](const Texture* texture) -> Rect2f
			{
				const float aspectRatio = texture->width / (float)texture->height;
				const Vector2f engineSize = 
				{ 
					aspectRatio * c_EngineHeight, 
					c_EngineHeight 
				};
				const float perSegment = c_XOffsetPerSegment;
				const float overhang = engineSize.x - perSegment;
				const float fromLeft = segmentIndex * perSegment;
				const float totalWidth = 
				{
					((amountOfSegments - 1) * perSegment) + overhang
				};
				const float left = -totalWidth / 2.0f;
				const float xPosition = left + fromLeft;
				return Rect2f({ xPosition, 0 }, engineSize);
			};	

		const Texture* texture = 
		{
			imageTextureLoader->LazyLoadTexture(c_TexturePath)
		};
		m_Sprite = std::make_unique<ImageSprite>(
			calculateEngineRect(texture),
			coordTransformer,
			programConstants,
			texture
		);
		m_Sprite->SetParent(parent);
	}

	void FloatingPlatformSegment::Update()
	{
		m_Sprite->Update();
	}

	void FloatingPlatformSegment::Draw() const
	{
		m_Sprite->Draw();
	}

	const float FloatingPlatformSegment::c_EngineHeight = 10 / 46.0f;
	const float FloatingPlatformSegment::c_XOffsetPerSegment = 0.169837f;
	const char* const FloatingPlatformSegment::c_TexturePath = 
	{
		"props/floatingPlatform.png"
	};


	FloatingPlatform::FloatingPlatform(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader
	)
	{
		const auto initializeEmpty = 	
			[&, this]()
			{
				const Vector2f viewportSize = 
				{
					coordTransformer->GetViewportSize()
				};
				const Vector2f enginePosition = 
				{
					(GetRandomFloat() - (1 / 2.0f)) * viewportSize.x,
					c_SpawningRange.ChooseYPosition()
				};
				m_Empty = std::make_unique<Empty>(
					enginePosition, 
					coordTransformer, 
					programConstants
				);
			};

		const auto generateSegmentCount = 
			[this]()
			{
				const float randomNumber = GetRandomFloat();
				
				const float start = c_MinimumSegmentCount;
				const float end = c_MaximumSegmentCount;
				
				const float result = start + (randomNumber * (end - start));
				return (size_t)result;
			};

		const auto initializeSegments = 
			[&, this]()
			{
				const size_t segmentCount = generateSegmentCount();
				for (size_t index = 0; index < segmentCount; index++)
				{
					m_Segments.emplace_back(
						m_Empty.get(),
						index,
						segmentCount,
						coordTransformer,
						programConstants,
						imageTextureLoader
					);
				}
			};

		initializeEmpty();
		initializeSegments();
	}

	const size_t FloatingPlatform::c_Count = 15;

	void FloatingPlatform::Update()
	{
		for (Segment& segment : m_Segments)
			segment.Update();
	}

	void FloatingPlatform::Draw() const
	{
		for (const Segment& segment : m_Segments)
			segment.Draw();
	}

	const SpawningRange FloatingPlatform::c_SpawningRange = 
	{
		.bottom=50, 
		.top=480 
	};

	const size_t FloatingPlatform::c_MinimumSegmentCount = 7;
	const size_t FloatingPlatform::c_MaximumSegmentCount = 14;

	
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
		const ProgramConstants* const programConstants
	) :
		m_WorldBounds(worldBounds),
		m_CoordTransformer(coordTransformer),
		m_ProgramConstants(programConstants),
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
			Logging::LogWarning(message);
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
			m_ProgramConstants,
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
			m_ProgramConstants,
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
					m_ProgramConstants->GetTargetFrametime()
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
					Logging::LogWarning(message.c_str());
					return;
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
					Logging::LogWarning(message.c_str());
					return;
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
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Rect2f* const worldBounds,
		const Direction* const movementDirection,
		const char* const texturePath,
		const SpawningRange spawningRange,
		const float speedMultiplier,
		const float speedRandomness,
		const Vector2f engineSize
	) :
		m_ProgramConstants(programConstants),
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
					programConstants,
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
		const float yPosition = m_SpawningRange.ChooseYPosition();
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
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Rect2f* const worldBounds,
		const Direction* const movementDirection
	) :
		Cloud(
			coordTransformer,
			programConstants,
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

	const char* const LowCloud::c_TexturePath = "props/lowCloud.png";
	const SpawningRange LowCloud::c_SpawningRange = 
	{
		.bottom=50, 
		.top=261.4863f 
	};
	const float LowCloud::c_SpeedMultiplier = 1;
	const float LowCloud::c_SpeedRandomness = 2 / 10.0f;
	const Vector2f LowCloud::c_EngineSize = { 2, 1 };


	HighCloud::HighCloud(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Rect2f* const worldBounds,
		const Direction* const movementDirection
	) :
		Cloud(
			coordTransformer,
			programConstants,
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

	const char* const HighCloud::c_TexturePath = "props/highCloud.png";

	const SpawningRange HighCloud::c_SpawningRange =
	{
		.bottom=256,
		.top=409.6f
	};
	const float HighCloud::c_SpeedMultiplier = 3 / 2.0f;
	const float HighCloud::c_SpeedRandomness = 2 / 10.0f;
	const Vector2f HighCloud::c_EngineSize =
	{
		7.556392017f * 2 / 3.0f, 2 / 3.0f
	};
}
