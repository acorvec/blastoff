#include "Powerup.h"

namespace BlastOff
{
	void Powerup::Update()
	{
		const auto updateRotation =
			[this]()
			{
				const float targetFrametime =
				{
					m_ProgramConfig->GetTargetFrametime()
				};
				m_RotationTick += 180.0f * targetFrametime;

				if (m_RotationTick >= 360)
					m_RotationTick -= 360;

				const float tickInRadians = ToRadians(m_RotationTick);
				const float rotation = m_OscillationScale * sinf(tickInRadians);

				m_Sprite->SetLocalRotation(rotation);
			};

		const auto updateCollection =
			[this]()
			{
				const float progress = m_CollectionTick / m_MaxCollectionTick;
				const float opacity = progress;
				const float scale = progress;

				m_Sprite->SetOpacity(opacity);
				m_Sprite->SetScale(scale);

				const float frametime = m_ProgramConfig->GetTargetFrametime();
				m_CollectionTick -= frametime;
			};

		updateRotation();

		if (WasRecentlyCollected())
			updateCollection();
	}

	void Powerup::Draw() const
	{
		if (!m_Sprite)
		{
			const char* const message =
			{
				"Powerup::Draw() failed: "
				"m_Sprite was not defined in sub-class implementation."
			};
			throw std::runtime_error(message);
		}
		m_Sprite->Draw();
	}

	void Powerup::OnCollection()
	{
		m_CollectionTick = m_MaxCollectionTick;
		m_IsCollected = true;
	}

	bool Powerup::IsCollected() const
	{
		return m_IsCollected;
	}

	Powerup::Powerup(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		Player* const player,
		ImageTextureLoader* const imageTextureLoader,
		const Vector2f enginePosition,
		const char* const texturePath,
		const float maxCollectionTick,
		const float oscillationScale,
		const Vector2f defaultEngineSize
	) :
		m_Player(player),
		m_ProgramConfig(programConfig),
		m_MaxCollectionTick(maxCollectionTick),
		m_OscillationScale(oscillationScale),
		m_DefaultEngineSize(defaultEngineSize)
	{
		const auto initializeSprite =
			[&, this]()
			{
				const Texture* const texture =
				{
					imageTextureLoader->LazyLoadTexture(texturePath)
				};
				const Rect2f engineRect(
					enginePosition,
					defaultEngineSize
				);
				m_Sprite = std::make_unique<ImageSprite>(
					engineRect,
					coordTransformer,
					programConfig,
					texture
				);
			};

		initializeSprite();
	}

	Circle2f Powerup::GetCircle() const
	{
		const Rect2f engineRect = m_Sprite->GetEngineRect();
		const Vector2f enginePosition = engineRect.GetPosition();

		const Vector2f engineSize = engineRect.GetSize();
		const float averageDimension = (engineSize.x + engineSize.y) / 2.0f;
		const float radius = averageDimension / 2.0f;

		return Circle2f(enginePosition, radius);
	}

	bool Powerup::CircleCollideWithPlayer() const
	{
		const Rect2f playerRect = m_Player->GetEngineRect();
		const Circle2f circle = GetCircle();

		return circle.CollideWithRect(playerRect);
	}

	bool Powerup::WasRecentlyCollected() const
	{
		return m_CollectionTick >= 0;
	}


	SpeedUpPowerup::SpeedUpPowerup(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		Player* const player,
		ImageTextureLoader* const imageTextureLoader,
		const Vector2f enginePosition
	) :
		Powerup(
			coordTransformer,
			programConfig,
			player,
			imageTextureLoader,
			enginePosition,
			c_TexturePath,
			c_MaxCollectionTick,
			c_OscillationScale,
			c_DefaultEngineSize
		)
	{

	}

	bool SpeedUpPowerup::CollideWithPlayer() const
	{
		return CircleCollideWithPlayer();
	}

	void SpeedUpPowerup::OnCollection()
	{
		Powerup::OnCollection();

		m_Player->RefillSpeedup();
	}

	const char* const SpeedUpPowerup::c_TexturePath = 
	{
		"powerup/SpeedUpPowerUp.png"
	};

	const float SpeedUpPowerup::c_MaxCollectionTick = 0.5f;
	const float SpeedUpPowerup::c_OscillationScale = 10;
	const Vector2f SpeedUpPowerup::c_DefaultEngineSize = { 1, 1 };


	FuelUpPowerup::FuelUpPowerup(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		Player* const player,
		ImageTextureLoader* const imageTextureLoader,
		const Vector2f enginePosition
	) :
		Powerup(
			coordTransformer,
			programConfig,
			player,
			imageTextureLoader,
			enginePosition,
			c_TexturePath,
			c_MaxCollectionTick,
			c_OscillationScale,
			c_DefaultEngineSize
		)
	{

	}

	bool FuelUpPowerup::CollideWithPlayer() const
	{
		return CircleCollideWithPlayer();
	}

	void FuelUpPowerup::OnCollection()
	{
		Powerup::OnCollection();

		m_Player->Refuel();
	}

	const char* const FuelUpPowerup::c_TexturePath = 
	{
		"powerup/FuelUpPowerUp.png"
	};

	const float FuelUpPowerup::c_MaxCollectionTick = 0.5f;
	const float FuelUpPowerup::c_OscillationScale = 10;
	const Vector2f FuelUpPowerup::c_DefaultEngineSize = { 1, 1 };
}
