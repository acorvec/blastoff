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
            const ProgramConfiguration* const programConfig,
    		ImageTextureLoader* const imageTextureLoader,
	    	TextTextureLoader* const textTextureLoader,
    		SoundLoader* const soundLoader,
	    	const Font* const font,
		    const Vector2i* const windowPosition,
    		const Vector2i* const windowSize		
        );

		virtual void Update();
		virtual void Draw() const;

	protected:
		using Constants = GameConstants;
		using Outcome = GameOutcome;
		using EndMenu = GameEndMenu;

		static const inline Constants c_Constants;

		unique_ptr<CoordinateTransformer> m_CoordinateTransformer = nullptr;
		unique_ptr<InputManager> m_InputManager = nullptr;

		Outcome m_Outcome = Outcome::None;
		Direction m_CloudMovementDirection = Direction::None;
		Vector2f m_CameraPosition = Vector2f::Zero();
		Rect2f m_WorldBounds = Rect2f::UnitRect();

		unique_ptr<CameraEmpty> m_CameraEmpty = nullptr;
		unique_ptr<Background> m_Background = nullptr;
		unique_ptr<Crag> m_Crag = nullptr;
		unique_ptr<Platform> m_Platform = nullptr;
		unique_ptr<Player> m_Player = nullptr;
		unique_ptr<GUIBar> m_FuelBar = nullptr;
		unique_ptr<GUIBar> m_SpeedupBar = nullptr;
		unique_ptr<BarLabel> m_FuelBarLabel = nullptr;
		unique_ptr<BarLabel> m_SpeedupBarLabel = nullptr;

		vector<SpeedUpPowerup> m_SpeedUpPowerups = {};
		vector<FuelUpPowerup> m_FuelUpPowerups = {};

		vector<LowCloud> m_LowClouds = {};
		vector<HighCloud> m_HighClouds = {};

		vector<Powerup*> m_AllPowerups = {};
		vector<Cloud*> m_AllClouds = {};

		const ProgramConfiguration* m_ProgramConfig = nullptr;
		const Font* m_Font = nullptr;

		ImageTextureLoader* m_ImageTextureLoader = nullptr;
		TextTextureLoader* m_TextTextureLoader = nullptr;

		virtual void ChooseOutcome(const Outcome outcome);
		void FinishConstruction(unique_ptr<InputManager> inputManager);

		float GetWorldEdge(const Direction side) const;
		bool LosingConditionsAreSatisfied() const;
	};

	struct PlayableGame : public Game
	{
		PlayableGame(
			const bool* const programIsMuted,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			SoundLoader* const soundLoader,
			const Callback& resetCallback,
			const Callback& muteUnmuteCallback,
			const Font* const font,
			const Vector2i* const windowPosition,
			const Vector2i* const windowSize
		);

		void ChooseOutcome(const Outcome outcome) override;

		void Update() override;
		void Draw() const override;

	protected:
		unique_ptr<GameEndMenu> m_WinMenu = nullptr;
		unique_ptr<GameEndMenu> m_LoseMenu = nullptr;
		unique_ptr<TopRightResetButton> m_TopRightResetButton = nullptr;
		unique_ptr<MuteButton> m_MuteButton = nullptr;

		const Sound* m_WinSound = nullptr;
		const Sound* m_LoseSound = nullptr;
		const Sound* m_EasterEggSound1 = nullptr;
		const Sound* m_EasterEggSound2 = nullptr;
	};

	struct Cutscene : public Game
	{
		Cutscene(
			const ProgramConfiguration* const programConfig,
    		ImageTextureLoader* const imageTextureLoader,
	    	TextTextureLoader* const textTextureLoader,
    		SoundLoader* const soundLoader,
			const Callback& resetCallback,
	    	const Font* const font,
		    const Vector2i* const windowPosition,
    		const Vector2i* const windowSize
		);

		void Update() override;

	protected:
		static const float c_MaxResetTick;
		float m_ResetTick = c_DeactivatedTick;

		Callback m_ResetCallback;

		bool ResetTimerIsActive() const;
	};
}
