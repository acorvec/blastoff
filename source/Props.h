#pragma once

#include "Utils.h"
#include "Graphics.h"
#include "Debug.h"

namespace BlastOff
{
	struct Crag
	{
		Crag(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader
		);

		void Draw() const;

	private:
		static const char* const c_TexturePath;

		unique_ptr<ImageSprite> m_Sprite;
	};

	struct Platform
	{
		Platform(
			const float platformHeight,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig, 
			ImageTextureLoader* const imageTextureLoader
		);

		void UpdateCollisionRect(const Vector2f playerSize);

		Vector2f GetEngineSize() const;

		Line2f GetTopCollisionLine() const;
		Line2f GetLeftCollisionLine() const;
		Line2f GetRightCollisionLine() const;

		void Draw() const;

	private:
		static const char* const c_TexturePath;
		unique_ptr<ImageSprite> m_Sprite;
	};
	
	struct BackgroundConfiguration
	{
		BackgroundConfiguration();

		Colour4i GetLowerAtmosphereColour() const;
		Colour4i GetMiddleAtmosphereColour() const;
		Colour4i GetUpperAtmosphereColour() const;

		float GetLowerAtmosphereSpriteTop() const;

	private:
		Colour4i m_LowerAtmosphereColour;
		Colour4i m_MiddleAtmosphereColour;
		Colour4i m_UpperAtmosphereColour;

		float m_LowerAtmosphereSpriteTop;
	};

	struct Background
	{
		Background(
			const Rect2f* const worldBounds,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig
		);

		void Update();
		void Draw() const;

	private:
		static const inline BackgroundConfiguration c_Config;

		Rect2f m_MostRecentWorldBounds = Rect2f::UnitRect();

		unique_ptr<GradientSprite> m_LowerAtmosphereSprite = nullptr;
		unique_ptr<GradientSprite> m_UpperAtmosphereSprite = nullptr;

		const Rect2f* m_WorldBounds = nullptr;
		const CoordinateTransformer* m_CoordTransformer = nullptr;
		const ProgramConfiguration* m_ProgramConfig = nullptr;

#if COMPILE_CONFIG_DEBUG
		int m_SpriteInitializationCount = 0;
#endif

		void InitializeSprites();

		void InitializeLowerAtmosphereSprite();
		void InitializeUpperAtmosphereSprite();
	};

	struct CloudSpawningRange
	{
		float bottom = 0;
		float top = 0;
	};

	struct Cloud
	{
		void Update();
		void Draw() const;

		bool DrawsAbovePlayer() const;

	protected:
		using SpawningRange = CloudSpawningRange;

		float m_RandomSpeedMultiplier = 0;
		bool m_DrawsAbovePlayer = false;

		const ProgramConfiguration* m_ProgramConfig = nullptr;
		const Rect2f* m_WorldBounds = nullptr;
		const Direction* m_MovementDirection = nullptr;

		unique_ptr<Sprite> m_Sprite = nullptr;

		Cloud(
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
		);

		void InitializeYPosition();
		float CalculateMaxInitXTranslation() const;
		float CalculateSpeed() const;

	private:
		float m_SpeedMultiplier = 0;
		float m_SpeedRandomness = 0;
		Vector2f m_EngineSize = Vector2f::Zero();
		SpawningRange m_SpawningRange = {};
	};

	struct LowCloud : public Cloud
	{
		LowCloud(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			const Rect2f* const worldBounds,
			const Direction* const movementDirection
		);

	private:
		static const char* const c_TexturePath;

		static const float c_SpeedMultiplier;
		static const float c_SpeedRandomness;
		static const Vector2f c_EngineSize;
		static const SpawningRange c_SpawningRange;
	};

	struct HighCloud : public Cloud
	{
		HighCloud(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			const Rect2f* const worldBounds,
			const Direction* const movementDirection
		);

	private:
		static const char* const c_TexturePath;

		static const float c_SpeedMultiplier;
		static const float c_SpeedRandomness;
		static const Vector2f c_EngineSize;
		static const SpawningRange c_SpawningRange;
	};
}
