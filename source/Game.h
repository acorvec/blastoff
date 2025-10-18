#pragma once

#include "Utils.h"
#include "Graphics.h"
#include "Player.h"
#include "Enums.h"
#include "GameConstants.h"
#include "Props.h"
#include "Powerup.h"
#include "GUI.h"
#include "Sound.h"

namespace BlastOff
{
	struct Game
	{
		Game(
            const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			Vector2f* const cameraPosition,
			const Font* const font
        );

		virtual void Update();
		virtual void Draw() const;

		static uint64_t GetWinCount() 
		{
			return m_WinCount;
		}

		static uint64_t GetLossCount()
		{
			return m_LossCount;
		}

	private:		
		CoordinateTransformer* m_CoordTransformer = nullptr;
		CameraEmpty* m_CameraEmpty = nullptr;
		Vector2f* m_CameraPosition = nullptr;

	protected:
		using Constants = GameConstants;
		using Outcome = GameOutcome;
		using EndMenu = GameEndMenu;

		virtual void ChooseOutcome(const Outcome outcome);
		void FinishConstruction(
			CoordinateTransformer* const coordTransformer,
			CameraEmpty* const cameraEmpty,
			unique_ptr<InputManager> inputManager
		);

		float GetWorldEdge(const Direction side) const;
		bool LosingConditionsAreSatisfied() const;

		static const inline Constants c_Constants;

		static const bool c_PrintOutcomeStatistics;

		static inline uint64_t m_WinCount = 0;
		static inline uint64_t m_LossCount = 0;

		unique_ptr<InputManager> m_InputManager = nullptr;

		Outcome m_Outcome = Outcome::None;
		Direction m_CloudMovementDirection = Direction::None;
		Rect2f m_WorldBounds = Rect2f::UnitRect();

		unique_ptr<Background> m_Background = nullptr;
		unique_ptr<Crag> m_Crag = nullptr;
		unique_ptr<SpawnPlatform> m_Platform = nullptr;
		unique_ptr<Player> m_Player = nullptr;
		unique_ptr<GUIBar> m_FuelBar = nullptr;
		unique_ptr<GUIBar> m_SpeedupBar = nullptr;
		unique_ptr<BarLabel> m_FuelBarLabel = nullptr;
		unique_ptr<BarLabel> m_SpeedupBarLabel = nullptr;

		vector<SpeedUpPowerup> m_SpeedUpPowerups = {};
		vector<FuelUpPowerup> m_FuelUpPowerups = {};
		vector<DownforcePowerup> m_DownforcePowerups = {};
		vector<FloatingPlatform> m_FloatingPlatforms = {};

		vector<LowCloud> m_LowClouds = {};
		vector<HighCloud> m_HighClouds = {};

		vector<Powerup*> m_AllPowerups = {};
		vector<Cloud*> m_AllClouds = {};

		const ProgramConstants* m_ProgramConstants = nullptr;
		const Font* m_Font = nullptr;

		ImageTextureLoader* m_ImageTextureLoader = nullptr;
		TextTextureLoader* m_TextTextureLoader = nullptr;
	};

	struct PlayableGame : public Game
	{
		PlayableGame(
			const bool* const programIsMuted,
			const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			SoundLoader* const soundLoader,
			const Callback& muteUnmuteUnmuteCallback,
			const Callback& resetCallback,
			const Callback& exitCallback,
			const Font* const font,
			const Vector2i* const windowPosition,
			const Vector2i* const windowSize
		);

		void ChooseOutcome(const Outcome outcome) override;

		void Update() override;
		void Draw() const override;

	protected:
		Vector2f m_CameraPosition = Vector2f::Zero();

		unique_ptr<CameraEmpty> m_CameraEmpty = nullptr;
		unique_ptr<CoordinateTransformer> m_CoordinateTransformer = nullptr;

		unique_ptr<GameEndMenu> m_WinMenu = nullptr;
		unique_ptr<GameEndMenu> m_LoseMenu = nullptr;
		unique_ptr<MuteButton> m_MuteButton = nullptr;
		unique_ptr<TopRightResetButton> m_ResetButton = nullptr;
		unique_ptr<TopRightExitButton> m_ExitButton = nullptr;

		const Sound* m_WinSound = nullptr;
		const Sound* m_LoseSound = nullptr;
		const Sound* m_EasterEggSound1 = nullptr;
		const Sound* m_EasterEggSound2 = nullptr;
	};

	struct Cutscene : public Game
	{
		Cutscene(
			const ProgramConstants* const programConstants,
			CoordinateTransformer* const coordTransformer,
			CameraEmpty* const cameraEmpty,
			ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			Vector2f* const cameraPosition,
			const Callback& resetCallback,
			const Font* const font
		);

		void Update() override;

	protected:
		bool ResetTimerIsActive() const;
		
		static const float c_MaxResetTick;
		float m_ResetTick = c_DeactivatedTick;

		Callback m_ResetCallback;
	};
}
