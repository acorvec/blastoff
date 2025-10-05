#pragma once

#include "Utils.h"
#include "Graphics.h"
#include "Player.h"

namespace BlastOff
{
	struct GUIBar
	{
		virtual ~GUIBar()
		{

		}

		const Sprite* GetBackingSprite() const;
		float GetSmoothStatisticValue() const;
		Vector2f GetEnergySpriteSize() const;

		virtual void Update();
		virtual void Draw() const;

	protected:
		using GetTextureFunction = function
			<const Texture* (ImageTextureLoader* const)>;

		static const char* const c_BackingTexturePath;

		static const float c_Height;
		static const float c_CornerMargin;
		static const float c_BetweenMargin;
		static const float c_SmoothingPeriod;

		static const Vector2f c_EnergySizeMultiplier;

		bool m_IsFirstUpdateCall = true;

		unique_ptr<ImageSprite> m_BackingSprite = nullptr;
		unique_ptr<ImageSprite> m_EnergySprite = nullptr;

		const CameraEmpty* m_CameraEmpty = nullptr;
		const ProgramConfiguration* m_ProgramConfig = nullptr;

		const float* m_StatisticValue = nullptr;
		float m_SmoothStatisticValue = 0;

		GUIBar(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const float* const statisticValue,
			const GetTextureFunction& energyGetTextureFunction
		);

		virtual void UpdateStatistic() = 0;

		static const Texture* LazyLoadBackingTexture
    		(ImageTextureLoader* const imageTextureLoader);
	};

	struct FuelBar : public GUIBar
	{
		FuelBar(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const Player* const player
		);

	private:
		static const char* const c_EnergyTexturePath;

		static const Texture* LazyLoadEnergyTexture
		(ImageTextureLoader* const imageTextureLoader);

		float m_StatisticValue = 0;
		const Player* m_Player = nullptr;

		void UpdateStatistic() override;
	};

	struct SpeedupBar : public GUIBar
	{
		SpeedupBar(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const Player* const player
		);

	private:
		static const char* const c_EnergyTexturePath;

		static const Texture* LazyLoadEnergyTexture
			(ImageTextureLoader* const imageTextureLoader);

		float m_StatisticValue = 0;
		const Player* m_Player = nullptr;

		void UpdateStatistic() override;
	};

	struct GUILabel
	{
		virtual ~GUILabel()
		{

		}

		virtual void Update();
		virtual void Draw() const;

	protected:
		unique_ptr<TextSprite> m_Sprite;

		GUILabel(
			const Vector2f enginePosition,
			const Colour4i colour,
			const float fontSize,
			const char* const message,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			TextTextureLoader* const textTextureLoader,
			const Font* const font
		);
	};

	enum class BarLabelType
	{
		Invalid = 0,

		AboveBacking,
		AboveEnergy
	};

	struct BarLabelColours
	{
		Colour4i aboveBacking = c_Black;
		Colour4i aboveEnergy = c_Grey;
	};

	struct BarLabelComponent : public GUILabel
	{
		using Type = BarLabelType;

		BarLabelComponent(
			const GUIBar* bar,
			const Type type,
			const Colour4i colour,
			const char* const message,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			TextTextureLoader* const textTextureLoader,
			const Font* const font
		);

		void Update() override;

	private:
		static const float c_FontSize;
		static const Vector2f c_Offset;

		Type m_Type;
		const GUIBar* m_Bar;
	};

	struct BarLabel
	{
		using Colours = BarLabelColours;

		BarLabel(
			const GUIBar* bar,
			const Colours colours,
			const char* const message,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			TextTextureLoader* const textTextureLoader,
			const Font* const font
		);

		void Update();
		void Draw() const;

	private:
		unique_ptr<BarLabelComponent> m_BackingLabel;
		unique_ptr<BarLabelComponent> m_EnergyLabel;
	};

	struct FuelBarLabel : public BarLabel
	{
		FuelBarLabel(
			const GUIBar* fuelBar,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			TextTextureLoader* const textTextureLoader,
			const Font* const font
		);

	private:
		static const char* const c_Message;
		static const BarLabelColours c_Colours;
	};

	struct SpeedupBarLabel : public BarLabel
	{
		SpeedupBarLabel(
			const GUIBar* speedupLabel,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConfiguration* const programConfig,
			TextTextureLoader* const textTextureLoader,
			const Font* const font
		);

	private:
		static const char* const c_Message;
		static const BarLabelColours c_Colours;
	};

	struct Button
	{
		void Translate(const Vector2f translation);

		virtual void Update();
		virtual void Draw() const;

	protected:
		using GetTextureFunction = function
			<const Texture* (ImageTextureLoader* const)>;

		bool m_IsSelected = false;
		bool m_IsClicked = false;
		bool m_HasBeenClicked = false;
		bool m_IsEnabled = true;

		Callback m_ClickCallback;
		unique_ptr<ImageSprite> m_Sprite = nullptr;

		const CoordinateTransformer* m_CoordTransformer = nullptr;
		const InputManager* m_InputManager = nullptr;

		Button(
			const Callback& clickCallback,
			const GetTextureFunction& unselectedGetTextureFunction,
			const GetTextureFunction& selectedGetTextureFunction,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f enginePosition,
			const Vector2f engineSize,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConfiguration* const programConfig
		);
		virtual ~Button()
		{

		}

	private:
		const Texture* m_UnselectedTexture = nullptr;
		const Texture* m_SelectedTexture = nullptr;
	};

	struct ResetButton : public Button
	{
		ResetButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			const Callback& resetCallback,
			const Vector2f enginePosition,
			const Sprite* const parent
		);

		void SetParent(const Sprite* const parent);

	protected:
		static const Vector2f c_Margins;
		static const Vector2f c_EngineSize;
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;

		static inline Texture m_UnselectedTexture = { 0 };
		static inline Texture m_SelectedTexture = { 0 };

		static const Texture* LazyLoadUnselectedTexture
			(ImageTextureLoader* const imageTextureLoader);
		static const Texture* LazyLoadSelectedTexture
			(ImageTextureLoader* const imageTextureLoader);
	};

	struct TopRightResetButton : public ResetButton
	{
		TopRightResetButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			const Callback& resetCallback,
			const CameraEmpty* const cameraEmpty
		);
		
		void SlideOut();
		void Update() override;

	protected:
		static const float c_MaxSlideOutTick;
		float m_SlideOutTick = c_DeactivatedTick;

		Vector2f m_StartingPosition = Vector2f::Zero();
		Vector2f m_EndingPosition = Vector2f::Zero();

		const ProgramConfiguration* m_ProgramConfig = nullptr;

		bool IsSlidingOut() const;
	};

    struct MuteButton : public Button
    {
        MuteButton(
            const bool* const programIsMuted,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConfiguration* const programConfig,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& muteCallback,
            const CameraEmpty* const cameraEmpty
        );

        void SlideOut();
        void Update() override;
        void Draw() const override;

    protected:
        static const Vector2f c_Margins;
        static const Vector2f c_EngineSize;
        static const Vector2f c_ActiveBarSize;
        static const char* const c_UnselectedTexturePath;
        static const char* const c_SelectedTexturePath;
        static const char* const c_ActiveBarTexturePath;

        static inline Texture m_UnselectedTexture = { 0 };
        static inline Texture m_SelectedTexture = { 0 };

        const bool* m_IsActive = nullptr;
        unique_ptr<ImageSprite> m_ActiveBar = nullptr;

        static const Texture* LazyLoadUnselectedTexture
            (ImageTextureLoader* const imageTextureLoader);
        static const Texture* LazyLoadSelectedTexture
            (ImageTextureLoader* const imageTextureLoader);
    };

	struct GameEndMenu
	{
		GameEndMenu(
			const Colour4i backingColour,
			const char* const messageText,
			const Callback& resetCallback,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const Font* const font
		);
		virtual ~GameEndMenu()
		{

		}

		void Enable();

		virtual void Update();
		virtual void Draw() const;

	private:
		static const float c_Roundness;
		static const float c_StrokeWidth;
		static const float c_MessageFontSize;
		static const float c_MaxSlideInTick;

		static const Vector2f c_Margins;
		static const Vector2f c_ResetButtonAdditionalOffset;

		bool m_IsEnabled = false;
		float m_SlideInTick = c_DeactivatedTick;

		Vector2f m_SlideInStartPosition = Vector2f::Zero();
		Vector2f m_SlideInEndPosition = Vector2f::Zero();

		const ProgramConfiguration* m_ProgramConfig = nullptr;

		unique_ptr<RoundedRectangleSprite> m_BackingFill = nullptr;
		unique_ptr<RoundedRectangleSprite> m_BackingStroke = nullptr;
		unique_ptr<TextSprite> m_Message = nullptr;
		unique_ptr<ResetButton> m_ResetButton = nullptr;
	};

	struct WinMenu : public GameEndMenu
	{
		WinMenu(
			const Callback& resetCallback,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const Font* const font
		);

	private:
		static const Colour4i c_BackingColour;
		static const char* const c_MessageText;
	};

	struct LoseMenu : public GameEndMenu
	{
		LoseMenu(
			const Callback& resetCallback,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConfiguration* const programConfig,
			ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const Font* const font
		);

	private:
		static const Colour4i c_BackingColour;
		static const char* const c_MessageText;
    };
}
