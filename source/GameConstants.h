#pragma once

#include "Utils.h"
#include "Debug.h"

namespace BlastOff
{
	struct GameConstants
	{
		GameConstants();

		bool GetLoadingScreenShown() const;

		float GetCameraYOffset() const;
		float GetWorldBoundHeight() const;
		float GetCragWidthMultiplier() const;
		float GetGravitationalAcceleration() const;
		float GetCollisionMultiplier() const;
		float GetPlatformHeight() const;

		size_t GetAmountOfLowClouds() const;
		size_t GetAmountOfHighClouds() const;
		size_t GetAmountOfFuelPowerups() const;
		size_t GetAmountOfSpeedPowerups() const;

		Vector2f GetLoadingScreenTextMargins() const;

#if COMPILE_CONFIG_DEBUG
		int GetPlayerFreezeKey() const;
		int GetPlayerTeleportKey() const;
		int GetEmptyPlayerFuelKey() const;
#endif
	private:
		bool m_LoadingScreenShown;

		float m_CameraYOffset;
		float m_WorldBoundHeight;
		float m_CragWidthMultiplier;
		float m_GravitationalAcceleration;
		float m_CollisionMultiplier;
		float m_PlatformHeight;

		size_t m_AmountOfLowClouds;
		size_t m_AmountOfHighClouds;
		size_t m_AmountOfFuelPowerups;
		size_t m_AmountOfSpeedPowerups;

		Vector2f m_LoadingScreenTextMargins;

#if COMPILE_CONFIG_DEBUG
		int m_PlayerFreezeKey = 0;
		int m_PlayerTeleportKey = 0;
		int m_EmptyPlayerFuelKey = 0;
#endif
	};
}