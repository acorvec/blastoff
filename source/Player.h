#pragma once

#include "Utils.h"
#include "Graphics.h"
#include "Enums.h"
#include "GameConstants.h"
#include "Props.h"
#include "InputManager.h"

namespace BlastOff
{
	struct PlayerConfig
	{
		PlayerConfig(const float targetFramerate);

		int GetRegularSmoothingFrames() const;
		int GetRotationSmoothingFrames() const;

		float GetSpaceshipHeight() const;
		float GetFlameFramerate() const;
		float GetThrustAccelerationMultiplier() const;
		float GetTerminalVelocity() const;
		float GetGroundedThreshold() const;
		float GetSpeedupMultiplier() const;
		float GetRunningOnFumesThreshold() const;

	private:
		int m_RegularSmoothingFrames;
		int m_RotationSmoothingFrames;

		float m_SpaceshipHeight;
		float m_FlameFramerate;
		float m_ThrustAccelerationMultiplier;
		float m_TerminalVelocity;
		float m_GroundedThreshold;
		float m_SpeedupMultiplier;
		float m_RunningOnFumesThreshold;
	};

	struct Player
	{
		Player(
			const GameOutcome* const gameOutcome,
			const Rect2f* const worldBounds,
			const SpawnPlatform* const platform,
			const CoordinateTransformer* const coordTransformer,
			const GameConstants* const gameConstants,
			const ProgramConstants* const programConstants,
			const InputManager* const inputManager,
			ImageTextureLoader* const imageTextureLoader
		);

		bool IsStationary() const;
		bool IsOutOfFuel() const;

		float GetFuelRatio() const;
		float GetSpeedUpRatio() const;

		void Update();
		void Draw() const;

		void Freeze();
		void Unfreeze();
		void SetFrozen(const bool isFrozen);
		void Teleport(const Vector2f position);
		void TeleportToY(const float yPosition);
		void EmptyFuel();

		void Refuel(const float amount);
		void RefillSpeedup(const float amount);
		void MultiplyVelocity(const float multiplier);

		Rect2f GetEngineRect() const;
		float GetEdgePosition(const Direction side) const;

		Vector2f GetVelocity() const;

	private:
		static const char* const c_SpaceshipTexturePath;
		static const char* const c_FlameTexturePath;

		static inline unique_ptr<const PlayerConfig> m_Config = nullptr;

		bool m_DidCollideHorizontally = false;
		bool m_BottomCollision = false;
		bool m_IsFrozen = false;

		int64_t m_FramesThrusted = c_DeactivatedTick;
		int64_t m_StationaryFrameCount = c_DeactivatedTick;

		float m_CurrentFuel = 0;
		float m_MaximumFuel = 0;

		float m_SpeedupTick = c_DeactivatedTick;
		float m_MaxSpeedupTick = 0;

		Vector2f m_Velocity = Vector2f::Zero();
		Vector2f m_ThrustAcceleration = Vector2f::Zero();

		unique_ptr<ImageSprite> m_Spaceship;
		unique_ptr<ImageSprite> m_SpaceshipFlame;

		const Rect2f* m_WorldBounds = nullptr;
		const GameOutcome* m_GameOutcome = nullptr;
		const SpawnPlatform* m_Platform = nullptr;
		const CoordinateTransformer* m_CoordTransformer = nullptr;
		const ProgramConstants* m_ProgramConstants = nullptr;
		const GameConstants* m_GameConstants = nullptr;
		const InputManager* m_InputManager = nullptr;

		bool IsThrusting() const;
		bool IsGoingTooFast() const;
		bool IsGrounded() const;
		bool IsRunningOnFumes() const;
		bool IsSpedUp() const;
	};
}
