#pragma once

#include "ProgramConstants.h"
#include "Utils.h"
#include "Graphics.h"
#include "Player.h"

namespace BlastOff
{
	struct Powerup
	{
		virtual bool CollideWithPlayer() const = 0;
		virtual void OnCollection();
		virtual void Update();
		virtual void Draw() const;

		bool IsCollected() const;

	protected:
		bool m_IsCollected = false;

		float m_CollectionTick = c_DeactivatedTick;
		float m_RotationTick = 0;
		
		const ProgramConstants* m_ProgramConstants = nullptr;

		Player* m_Player = nullptr;

		unique_ptr<Sprite> m_Sprite = nullptr;

		Powerup(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			Player* const player,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f enginePosition,
			const char* const texturePath,
			const float maxCollectionTick,
			const float oscillationScale,
			const Vector2f defaultEngineSize
		);

		Circle2f GetCircle() const;
		bool CircleCollideWithPlayer() const;

		bool WasRecentlyCollected() const;

	private:
		float m_MaxCollectionTick = 0;
		float m_OscillationScale = 0;
		Vector2f m_DefaultEngineSize = Vector2f::Zero();
	};

	struct SpeedUpPowerup : public Powerup
	{
		SpeedUpPowerup(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			Player* const player,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f enginePosition
		);

		bool CollideWithPlayer() const override;
		void OnCollection() override;
		
		static const size_t c_Count;
		static const bool c_SpawningEnabled;

	private:
		static const char* const c_TexturePath;

		static const float c_MaxCollectionTick;
		static const float c_OscillationScale;
		static const float c_EnergyAmount;
		static const Vector2f c_DefaultEngineSize;
	};

	struct FuelUpPowerup : public Powerup
	{
		FuelUpPowerup(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			Player* const player,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f enginePosition
		);

		bool CollideWithPlayer() const override;
		void OnCollection() override;
		
		static const size_t c_Count;
		static const bool c_SpawningEnabled;
	
	private:
		static const char* const c_TexturePath;

		static const float c_MaxCollectionTick;
		static const float c_OscillationScale;
		static const float c_EnergyAmount;
		static const Vector2f c_DefaultEngineSize;
	};

	struct DownforcePowerup : public Powerup
	{
		DownforcePowerup(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			Player* const player,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f enginePosition
		);

		bool CollideWithPlayer() const override;
		void OnCollection() override;
		
		static const size_t c_Count;
		static const bool c_SpawningEnabled;

	private:
		static const char* const c_TexturePath;
		static const float c_MaxCollectionTick;
		static const float c_OscillationScale;
		static const float c_ForceMultiplier;
		static const Vector2f c_DefaultEngineSize;
	};
}
