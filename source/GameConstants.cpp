#include "GameConstants.h"

namespace BlastOff
{
	GameConstants::GameConstants() :
		m_LoadingScreenShown(true),
		m_AmountOfLowClouds(30),
		m_AmountOfHighClouds(30),
		m_AmountOfFuelPowerups(10),
		m_AmountOfSpeedPowerups(10),
		m_CameraYOffset(-2 / 3.0f),
		m_WorldBoundHeight(512),
		m_CragWidthMultiplier(2),
		m_GravitationalAcceleration(5),
		m_CollisionMultiplier(3 / 4.0f),
		m_PlatformHeight(3 / 2.0f)
	{
#if COMPILE_CONFIG_DEBUG
		m_PlayerFreezeKey = KEY_F;
		m_PlayerTeleportKey = KEY_T;
		m_EmptyPlayerFuelKey = KEY_E;
#endif
	}

	bool GameConstants::GetLoadingScreenShown() const
	{
		return m_LoadingScreenShown;
	}

	float GameConstants::GetCameraYOffset() const
	{
		return m_CameraYOffset;
	}

	float GameConstants::GetWorldBoundHeight() const
	{
		return m_WorldBoundHeight;
	}

	float GameConstants::GetCragWidthMultiplier() const
	{
		return m_CragWidthMultiplier;
	}

	float GameConstants::GetGravitationalAcceleration() const
	{
		return m_GravitationalAcceleration;
	}

	float GameConstants::GetCollisionMultiplier() const
	{
		return m_CollisionMultiplier;
	}

	float GameConstants::GetPlatformHeight() const
	{
		return m_PlatformHeight;
	}

	size_t GameConstants::GetAmountOfLowClouds() const
	{
		return m_AmountOfLowClouds;
	}

	size_t GameConstants::GetAmountOfHighClouds() const
	{
		return m_AmountOfHighClouds;
	}

	size_t GameConstants::GetAmountOfFuelPowerups() const
	{
		return m_AmountOfFuelPowerups;
	}

	size_t GameConstants::GetAmountOfSpeedPowerups() const
	{
		return m_AmountOfSpeedPowerups;
	}

#if COMPILE_CONFIG_DEBUG
	int GameConstants::GetPlayerFreezeKey() const
	{
		return m_PlayerFreezeKey;
	}

	int GameConstants::GetPlayerTeleportKey() const
	{
		return m_PlayerTeleportKey;
	}

	int GameConstants::GetEmptyPlayerFuelKey() const
	{
		return m_EmptyPlayerFuelKey;
	}
#endif
}