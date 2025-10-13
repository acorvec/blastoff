#pragma once

#include "Enums.h"
#include "ProgramConstants.h"
#include "Utils.h"
#include "Graphics.h"
#include "Player.h"
#include "Settings.h"
#include "raylib.h"
#include <optional>

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
		static const char* const c_BackingTexturePath;

		static const float c_Height;
		static const float c_CornerMargin;
		static const float c_BetweenMargin;
		static const float c_SmoothingPeriod;

		static const Vector2f c_EnergySizeMultiplier;

		bool m_IsFirstUpdateCall = true;

		const float* m_StatisticValue = nullptr;
		float m_SmoothStatisticValue = 0;

		unique_ptr<ImageSprite> m_BackingSprite = nullptr;
		unique_ptr<ImageSprite> m_EnergySprite = nullptr;

		const CameraEmpty* m_CameraEmpty = nullptr;
		const ProgramConstants* m_ProgramConstants = nullptr;

		GUIBar(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const float* const statisticValue,
			const char* const energyTexturePath
		);

		virtual void UpdateStatistic() = 0;
	};

	struct FuelBar : public GUIBar
	{
		FuelBar(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const Player* const player
		);

	private:
		static const char* const c_EnergyTexturePath;

		float m_StatisticValue = 0;
		const Player* m_Player = nullptr;

		void UpdateStatistic() override;
	};

	struct SpeedupBar : public GUIBar
	{
		SpeedupBar(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const Player* const player
		);

	private:
		static const char* const c_EnergyTexturePath;

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
		unique_ptr<TextLineSprite> m_Sprite;

		GUILabel(
			const Vector2f enginePosition,
			const Colour4i colour,
			const float fontSize,
			const char* const message,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
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
			const ProgramConstants* const programConstants,
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
			const ProgramConstants* const programConstants,
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
			const ProgramConstants* const programConstants,
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
			const ProgramConstants* const programConstants,
			TextTextureLoader* const textTextureLoader,
			const Font* const font
		);

	private:
		static const char* const c_Message;
		static const BarLabelColours c_Colours;
	};

	struct Button
	{
		virtual ~Button()
		{

		}

		void Translate(const Vector2f translation);
		void SetParent(const Sprite* const parent);
		void UseUnselectedTexture();

		virtual void Disable();
		virtual void Enable();
		virtual void UpdateOpacity();
		virtual void Update();
		virtual void Draw() const;

	protected:
		bool m_IsSelected = false;
		bool m_ShouldShowClickedSprite = false;
		bool m_HasBeenClicked = false;
		bool m_IsEnabled = true;
		bool m_HasJustEnabled = true;

		Callback m_ClickCallback;
		unique_ptr<ImageSprite> m_Sprite = nullptr;

		const CoordinateTransformer* m_CoordTransformer = nullptr;
		const InputManager* m_InputManager = nullptr;

		const float* m_ParentOpacity = nullptr;
		const Texture* m_UnselectedTexture = nullptr;
		const Texture* m_SelectedTexture = nullptr;
		const Texture* m_ClickedTexture = nullptr;

		Button(
			const Callback& clickCallback,
			const char* const unselectedTexturePath,
			const char* const selectedTexturePath,
			const char* const clickedTexturePath,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f enginePosition,
			const Vector2f engineSize,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
			const float* const parentOpacity = nullptr
		);
	};

	struct YesButton : Button
	{
		YesButton(
			const Callback& yesCallback,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f bottomRightCorner,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants
		);

	protected:
		static const int c_ButtonIndex;
		
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;
	};

	struct SettingsMenuCancelButton : public Button
	{
		SettingsMenuCancelButton(
			const Callback& cancelCallback,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f bottomRightCorner,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants
		);

	protected:
		static const int c_ButtonIndex;

		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;
	};

	struct NoButton : Button
	{
		NoButton(
			const Callback& noCallback,
			ImageTextureLoader* const imageTextureLoader,
			const Vector2f bottomRightCorner,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants
		);

	protected:
		static const int c_ButtonIndex;
		
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;
	};

	struct Theme
	{
		static const Theme c_DarkTheme;

		float outerBackingRoundness;
		float outerBackingStrokeWidth;
		ShapeColours outerBackingColours;
		Vector2f outerMargins;

		float innerBackingRoundness;
		float innerBackingStrokeWidth;
		ShapeColours innerBackingColours;
		Vector2f innerMargins;

		Colour4i textColour;
	};

	struct ThemedBacking
	{
		ThemedBacking(
			const Vector2f innerSize,
			const Theme* const theme,
			const Sprite* const parent,
			const float* const parentOpacity,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader
		);

		Vector2f CalculateBottomRightCorner() const;
		Vector2f GetEngineSize() const;

		void UpdateOpacity();
		void Update();
		void Draw() const;

	private:
		const float* const m_ParentOpacity = nullptr;

		unique_ptr<RoundedRectangleSprite> m_OuterBackingFill = nullptr;
		unique_ptr<RoundedRectangleSprite> m_OuterBackingStroke = nullptr;
		unique_ptr<RoundedRectangleSprite> m_InnerBackingFill = nullptr;
		unique_ptr<RoundedRectangleSprite> m_InnerBackingStroke = nullptr;
	};

	struct SlideState
	{
		SlideState(
			const Vector2f startingPosition,
			const Vector2f endingPosition,
			const float maxTick,
			Sprite* sprite,
			const ProgramConstants* const programConstants
		);

		Vector2f GetStartingPosition() const;
		bool HasJustFinished() const;
		
		void Slide(const float waitInSeconds = 0);
		void SwapPositions();
		void Update();

	protected:
		bool m_HasJustFinished = false;
		float m_MaxSlideTick = 0;
		float m_SlideTick = c_DeactivatedTick;
		float m_WaitTick = c_DeactivatedTick;

		Vector2f m_StartingPosition = Vector2f::Zero();
		Vector2f m_EndingPosition = Vector2f::Zero();

		const ProgramConstants* m_ProgramConstants;
		Sprite* m_Sprite = nullptr;

		bool IsWaiting() const;
		bool IsSliding() const;
	};

	struct BackgroundTint
	{
		BackgroundTint(
			const Sprite* const parent,
			const Colour4i colour,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			const Direction slideDirection,
			const float slideLength
		);

		void Enable();
		void Update();
		void Draw() const;

	private:
		static const float c_SlideInWait;

		bool m_IsEnabled = false;
	
		unique_ptr<RectangleSprite> m_Sprite = nullptr;
		unique_ptr<SlideState> m_SlideState = nullptr;
	};

	struct ConfirmationDialogue
	{
		bool IsEnabled() const;

		virtual void Enable();
		virtual void Disable();

		virtual void Update();
		void Draw() const;

	protected:
		static const float c_FontSize;
		static const float c_LineSpacing;

		static const Colour4i c_BackgroundTintColour;
		static const Direction c_BackgroundTintSlideDirection;
		static const float c_BackgroundTintSlideLength;
		
		static const inline float c_Opacity = 1;
		
		bool m_IsEnabled = false;
		
		unique_ptr<Empty> m_Empty = nullptr;
		unique_ptr<BackgroundTint> m_BackgroundTint = nullptr;
		unique_ptr<ThemedBacking> m_Backing = nullptr;
		unique_ptr<TextSprite> m_Message = nullptr;
		unique_ptr<Button> m_YesButton = nullptr;
		unique_ptr<Button> m_CancelButton = nullptr;
		unique_ptr<Button> m_NoButton = nullptr;

		ConfirmationDialogue(
			const Callback& yesCallback,
			const Callback& cancelCallback,
			const Callback& noCallback,
			const char* const message,
			const Vector2f enginePosition,
			const Sprite* const parent,
			const Theme* const theme,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			const InputManager* const inputManager,
			const Font* const font,
			TextTextureLoader* const textTextureLoader,
			ImageTextureLoader* const imageTextureLoader
		);
	};

	struct TopRightButton
	{
		static const Vector2f c_Margins;
		static const Vector2f c_EngineSize;

		static constexpr Vector2f CalculateOffsetByIndex(const int buttonIndex)
		{
			const float offsetY = buttonIndex * (c_Margins.y + c_EngineSize.y);
			return Vector2f{ 0, offsetY };
		}
	};

	struct CenterMenuButton
	{
		static const Vector2f c_Margins;
		static const Vector2f c_EngineSize;
		static const Vector2f c_AdditionalOffset;

		static constexpr Vector2f CalculateOffsetByIndex(const int buttonIndex)
		{
			const float offsetX = 
			{
				-buttonIndex * (c_Margins.y + c_EngineSize.y)
			};
			return Vector2f{ offsetX, 0 };
		}
	};

	struct MainMenuButton
	{
		static const Vector2f c_Margins;
		static const Vector2f c_EngineSize;
	};

    struct MuteButton : public Button
    {
        MuteButton(
            const bool* const programIsMuted,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& muteUnmuteCallback,
            const Sprite* const parent
        );

        void SlideOut();
        void Update() override;
        void Draw() const override;

    protected:
		static const int c_ButtonIndex;

        static const Vector2f c_ActiveBarSize;

        static const char* const c_UnselectedTexturePath;
        static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;
        static const char* const c_ActiveBarTexturePath;

        const bool* m_IsActive = nullptr;
        unique_ptr<ImageSprite> m_ActiveBar = nullptr;
	};

	struct ResetButton : public Button
	{
		ResetButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader,
			const Callback& resetCallback,
			const Vector2f enginePosition,
			const Sprite* const parent
		);

		void SetParent(const Sprite* const parent);

	protected:
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;
	};

	struct TopRightResetButton : public ResetButton
	{
		TopRightResetButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader,
			const Callback& resetCallback,
			const CameraEmpty* const cameraEmpty
		);

		void SlideOut();
		void Update() override;
	
	protected:
		static const int c_ButtonIndex;
		static const float c_SlideOutWait;
		static const float c_MaxSlideOutTick;
		
		unique_ptr<SlideState> m_SlideState = nullptr;
	};

	// ExitButton adds no extra functionality to Button
	using ExitButton = Button;

	struct TopRightExitButton : public ExitButton
	{
		TopRightExitButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& exitCallback,
            const Sprite* const parent,
			const ProgramState menuType
		);
		
		void SlideOut();
		void Update() override;

	protected:
		static const int c_ButtonIndexInGame;
		static const int c_ButtonIndexInSettingsMenu;
		static const float c_SlideOutWait;
		static const float c_MaxSlideOutTick;
		
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;

		unique_ptr<SlideState> m_SlideState = nullptr;
	};

	struct PlayButton : public Button
	{
		PlayButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& playCallback,
            const CameraEmpty* const cameraEmpty
		);

	protected:
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;
	};

	struct SettingsButton : public Button
	{
		SettingsButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& settingsCallback,
            const CameraEmpty* const cameraEmpty
		);

	protected:
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;
	};

	struct MainMenuExitButton : public ExitButton
	{
		MainMenuExitButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& exitCallback,
            const CameraEmpty* const cameraEmpty
		);

	protected:
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;

		Vector2f m_Margins = Vector2f::Zero();
	};

	struct CenterMenuResetButton : ResetButton
	{
		CenterMenuResetButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& resetCallback,
            const Sprite* const parent,
			const Vector2f bottomRightCorner
		);

	private:
		static const int c_ButtonIndex;
	};

	struct CenterMenuExitButton : ExitButton
	{
		CenterMenuExitButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& exitCallback,
            const Sprite* const parent,
			const Vector2f bottomRightCorner,
			const float* const parentOpacity = nullptr
		);

	private:
		static const int c_ButtonIndex;
		
		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;		
	};

	struct GameEndMenu
	{
		GameEndMenu(
			const Colour4i backingColour,
			const char* const messageText,
			const Callback& resetCallback,
			const Callback& exitCallback,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
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
		static const float c_SlideInWait;
		static const float c_MaxSlideInTick;

		static const Vector2f c_Margins;
		static const Vector2f c_ResetButtonAdditionalOffset;

		bool m_IsEnabled = false;

		unique_ptr<Empty> m_Empty = nullptr;
		unique_ptr<RoundedRectangleSprite> m_BackingFill = nullptr;
		unique_ptr<RoundedRectangleSprite> m_BackingStroke = nullptr;
		unique_ptr<TextLineSprite> m_Message = nullptr;
		unique_ptr<Button> m_ResetButton = nullptr;
		unique_ptr<Button> m_ExitButton = nullptr;
		unique_ptr<SlideState> m_SlideState = nullptr;
	};

	struct WinMenu : public GameEndMenu
	{
		WinMenu(
			const Callback& resetCallback,
			const Callback& exitCallback,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
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
			const Callback& exitCallback,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
			ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			const CameraEmpty* const cameraEmpty,
			const Font* const font
		);

	private:
		static const Colour4i c_BackingColour;
		static const char* const c_MessageText;
    };

    struct MainMenu
    {
        MainMenu(
            const ProgramConstants* const programConstants,
            const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const CameraEmpty* const cameraEmpty,
            ImageTextureLoader* const imageTextureLoader,
            TextTextureLoader* const textTextureLoader,
            const Callback& playCallback,
            const Callback& settingsCallback,
            const Callback& exitCallback,
            const Font* const font,
            const Vector2i* const windowPosition,
            const Vector2i* const windowSize
        );

        void Update();
        void Draw() const;

    private:
        unique_ptr<Button> m_PlayButton = nullptr;
        unique_ptr<Button> m_SettingsButton = nullptr;
        unique_ptr<Button> m_ExitButton = nullptr;

        const ProgramConstants* m_ProgramConstants = nullptr;
		const InputManager* m_InputManager = nullptr;
        const Font* m_Font = nullptr;
        const CoordinateTransformer* m_CoordTransformer = nullptr;
        const CameraEmpty* m_CameraEmpty = nullptr;
        const Vector2i* m_WindowPosition = nullptr;
        const Vector2i* m_WindowSize = nullptr;
        
        ImageTextureLoader* m_ImageTextureLoader = nullptr;
        TextTextureLoader* m_TextTextureLoader = nullptr;
    };

	struct SlideBarColours
	{
		struct HandleColours
		{
			ShapeColours unselected;
			ShapeColours selected;
			ShapeColours clicked;
		};

		HandleColours handle;
		ShapeColours backing;
	};

	template<
		typename Num,
		typename = typename std::enable_if<std::is_arithmetic<Num>::value, Num>
	>
	struct SlideBar
	{
		using Colours = SlideBarColours;

		SlideBar(
			const Vector2f enginePosition,
			const Vector2f backingSize,
			const Vector2f handleSize,
			const float strokeWidth,
			const float backingRoundness,
			const float handleRoundness,
			const Num startValue,
			const Num minimum,
			const Num maximum,
			const Colours colours,
			const Sprite* const parent,
			const float* const parentOpacity,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
			const optional<Num> stepSize = std::nullopt
		) :
			m_Value(startValue),
			m_Minimum(minimum),
			m_Maximum(maximum),
			m_StepSize(stepSize),
			m_Colours(colours),
			m_ParentOpacity(parentOpacity),
			m_InputManager(inputManager)
		{
			const auto checkBounds = 
				[&]()
				{
					m_Value = std::max(m_Value, m_Minimum);
					m_Value = std::min(m_Value, m_Maximum);
				};
				
			const auto createBacking = 
				[&, this]()
				{
					const Rect2f backingRect(enginePosition, backingSize);
					m_BackingFill = std::make_unique<RoundedRectangleSprite>(
						backingRect,
						colours.backing.fill,
						backingRoundness,
						coordTransformer,
						programConstants
					);
					m_BackingFill->SetParent(parent);

					m_BackingStroke = std::make_unique<RoundedRectangleSprite>(
						backingRect,
						colours.backing.stroke,
						backingRoundness,
						coordTransformer,
						programConstants,
						strokeWidth
					);
					m_BackingStroke->SetParent(m_BackingFill.get());
				};

			const auto createHandle = 
				[&, this]()
				{
					const Rect2f handleRect(Vector2f::Zero(), handleSize);
					const ShapeColours& defaultColourSet = 
					{
						m_Colours.handle.unselected
					};

					m_HandleFill = std::make_unique<RoundedRectangleSprite>(
						handleRect,
						defaultColourSet.fill,
						handleRoundness,
						coordTransformer,
						programConstants
					);
					m_HandleFill->SetParent(m_BackingFill.get());

					m_HandleStroke = std::make_unique<RoundedRectangleSprite>(
						handleRect,
						defaultColourSet.stroke,
						handleRoundness,
						coordTransformer,
						programConstants,
						strokeWidth
					);
					m_HandleStroke->SetParent(m_HandleFill.get());
				};

			checkBounds();			
			createBacking();
			createHandle();
			InitializeHandlePosition();
		}

		float GetValue() const
		{
			return m_Value;
		}

		float GetBottomEdgePosition() const
		{
			return m_HandleStroke->GetEdgePosition(Direction::Down);
		}

		float GetWidth() const
		{
			const Rect2f engineRect = m_BackingStroke->GetEngineRect();
			return engineRect.w;
		}

		void UpdateOpacity()
		{
			m_BackingFill->SetOpacity(*m_ParentOpacity);
			m_BackingStroke->SetOpacity(*m_ParentOpacity);

			m_HandleFill->SetOpacity(*m_ParentOpacity);
			m_HandleStroke->SetOpacity(*m_ParentOpacity);
		}

		void Update()
		{
			const auto updateHandleFlags = 
				[this]()
				{
					const Vector2f engineMouse = 
					{
						m_InputManager->CalculateMousePosition()
					};
					const Sprite* handleRoot = GetHandleRoot();
					const Rect2f handleRect = handleRoot->CalculateRealRect();
					m_HandleIsSelected = 
					{
						handleRect.CollideWithPoint(engineMouse)
					};

					constexpr int buttonEnum = MOUSE_BUTTON_LEFT;
					const bool mouseReleased =
					{
						m_InputManager->GetMouseButtonReleased(buttonEnum)
					};
					const bool mouseClicked = 
					{
						m_InputManager->GetMouseButtonPressed(buttonEnum)
					};
					if (mouseReleased)
						m_HandleIsClicked = false;

					m_HandleIsSelected = false;

					if (m_HandleIsSelected && mouseClicked)
						m_HandleIsClicked = true;
					else if (mouseClicked)
					{
						// seek if the player clicks on the backing instead
						const Sprite* backingRoot = GetBackingRoot();
						const Rect2f backingRect = backingRoot->CalculateRealRect();
						m_HandleIsClicked = 
						{
							backingRect.CollideWithPoint(engineMouse)
						};
					}
				};

			const auto updateHandleColours =
				[this]()
				{
					const auto& colours = m_Colours.handle;
					if (m_HandleIsClicked)
					{
						m_HandleFill->SetColour(colours.clicked.fill);
						m_HandleStroke->SetColour(colours.clicked.stroke);
					}
					else if (m_HandleIsSelected)
					{
						m_HandleFill->SetColour(colours.selected.fill);
						m_HandleStroke->SetColour(colours.selected.stroke);
					}
					else
					{
						m_HandleFill->SetColour(colours.unselected.fill);
						m_HandleStroke->SetColour(colours.unselected.stroke);
					}
				};

			const auto clampHandlePosition = 
				[this](const float mouseX) -> float
				{
					const float left = 
					{
						m_BackingFill->GetEdgePosition(Direction::Left)
					};
					const float right = 
					{
						m_BackingFill->GetEdgePosition(Direction::Right)
					};
					const float reverseLerpResult = ReverseLerp(
						left, 
						right, 
						mouseX
					);
					if (reverseLerpResult < 0)
						return left;
					else if (reverseLerpResult > 1)
						return right;
					else
						return mouseX;
				};

			const auto calculateSnappedValue =
				[this](const Num beforeSnapping) -> Num
				{
					const float result = RoundToFraction(
						(float)beforeSnapping, 
						(float)(*m_StepSize)
					);
					return (Num)result;
				};

			const auto updateHandlePosition = 
				[&, this]()
				{
					const Vector2f engineMouse = 
					{
						m_InputManager->CalculateMousePosition()
					};
					const float beforeSnapping = 
					{
						clampHandlePosition(engineMouse.x)
					};
					const float left = 
					{
						m_BackingFill->GetEdgePosition(Direction::Left)
					};
					const float right = 
					{
						m_BackingFill->GetEdgePosition(Direction::Right)
					};
					const float progress = ReverseLerp(
						left, 
						right, 
						beforeSnapping
					);
					m_Value = (progress * (m_Maximum - m_Minimum)) + m_Minimum;
					if (m_StepSize)
						m_Value = calculateSnappedValue(m_Value);
					InitializeHandlePosition();
				};

			const auto updateHandle =
				[&, this]()
				{
					updateHandleFlags();
					updateHandleColours();

					if (m_HandleIsClicked)
						updateHandlePosition();

					m_HandleFill->Update();
					m_HandleStroke->Update();
				};

			updateHandle();
			UpdateOpacity();

			m_BackingFill->Update();
			m_BackingStroke->Update();
		}

		void Draw() const
		{
			m_BackingFill->Draw();
			m_BackingStroke->Draw();

			m_HandleFill->Draw();
			m_HandleStroke->Draw();
		}

	protected:
		void InitializeHandlePosition()
		{
			const float left = 
			{
				m_BackingFill->GetEdgePosition(Direction::Left)
			};
			const float right = 
			{
				m_BackingFill->GetEdgePosition(Direction::Right)
			};
			const float progress = 
			{
				(m_Value - m_Minimum) / (float)(m_Maximum - m_Minimum)
			};

			const float a = left, b = right, t = progress;
			const float engineX = Lerp(left, right, progress);

			Sprite* handleRoot = GetHandleRoot();
			const Rect2f localRect = handleRoot->GetEngineRect();
			handleRoot->SetLocalPosition({ engineX, localRect.y });
		}

		RoundedRectangleSprite* GetHandleRoot() const
		{
			return m_HandleFill.get();
		}

		RoundedRectangleSprite* GetBackingRoot() const
		{
			return m_BackingFill.get();
		}

		bool m_HandleIsClicked = false;
		bool m_HandleIsSelected = false;

		Num m_Value = 0;
		Num m_Minimum = 0;
		Num m_Maximum = 0;

		optional<Num> m_StepSize = 0;
		Colours m_Colours = { 0 };

		const float* m_ParentOpacity = nullptr;
		const InputManager* const m_InputManager = nullptr;
		
		unique_ptr<RoundedRectangleSprite> m_BackingFill = nullptr;
		unique_ptr<RoundedRectangleSprite> m_BackingStroke = nullptr;

		unique_ptr<RoundedRectangleSprite> m_HandleFill = nullptr;
		unique_ptr<RoundedRectangleSprite> m_HandleStroke = nullptr;
	};

	struct SettingsMenuConfirmationDialogue : public ConfirmationDialogue
	{
		SettingsMenuConfirmationDialogue(
			const Callback& yesCallback,
			const Callback& cancelCallback,
			const Callback& noCallback,
			const Sprite* const parent,
			const Theme* const backingTheme,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			const InputManager* const inputManager,
			const Font* const font,
			TextTextureLoader* const textTextureLoader,
			ImageTextureLoader* const imageTextureLoader
		);

		void Enable() override;
		void Disable() override;
		void Update() override;

	protected:
		static const float c_MaxSlideInTick;
		static const float c_SlideWait;
		static const Vector2f c_EnginePosition;
		static const char* const c_Message;

		bool m_IsSlidingOut = false;
		Vector2f m_OffScreenPosition = Vector2f::Zero();

		unique_ptr<SlideState> m_SlideState = nullptr;
	};

	template<
		typename Num,
		typename = typename std::enable_if<std::is_arithmetic<Num>::value, Num>
	>
	struct SettingsMenuSlideBar : public SlideBar<Num>
	{
		using Colours = SlideBarColours;

		SettingsMenuSlideBar(
			const Vector2f enginePosition,
			const float* const parentOpacity,
			const Num startValue,
			const Num minimum,
			const Num maximum,
			const Sprite* const parent,
			Settings* const settings,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
			const optional<Num> stepSize = std::nullopt
		) :
			SlideBar<Num>(
				enginePosition,
				c_BackingSize,
				c_HandleSize,
				c_StrokeWidth,
				c_BackingRoundness,
				c_HandleRoundness,
				startValue,
				minimum,
				maximum,
				c_Colours,
				parent,
				parentOpacity,
				coordTransformer,
				inputManager,
				programConstants,
				stepSize
			),
			m_Settings(settings)
		{

		}
		
	protected:		
		static constexpr float c_BackingRoundness = 1 / 10.0f;
		static constexpr float c_HandleRoundness = 1 / 10.0f;
		static constexpr float c_StrokeWidth = 2 / 44.0f;

		static constexpr Vector2f c_HandleSize = { 1 / 6.0f, 1 / 3.0f };
		static constexpr Vector2f c_BackingSize = { 5 / 2.0f, 3 / 20.0f };
		static constexpr SlideBarColours c_Colours = 
		{
			.handle = 
			{
				.unselected = { .stroke = c_Black, .fill = Colour4i(0xA0) },
				.selected =
				{
					.stroke = c_Black, 
					.fill = Colour4i(0xC0)
				},
				.clicked = { .stroke = c_Black, .fill=Colour4i(0xE0) }
			},
			.backing = { .stroke = c_Black, .fill = c_White }
		};

		Settings* m_Settings;
	};

	struct VolumeSlideBar : public SettingsMenuSlideBar<float>
	{
		VolumeSlideBar(
			Settings* const settings,
			const float* const parentOpacity,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants
		);

	protected:
		static const float c_Minimum;
		static const float c_Maximum;
		static const Vector2f c_EnginePosition;
	};

	struct WindowSizeSlideBar : public SettingsMenuSlideBar<int>
	{
		WindowSizeSlideBar(
			Settings* const settings,
			const int windowSizeIncrement,
			const float* const parentOpacity,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants
		);

	protected:
		static const int c_Minimum;
		static const Vector2f c_EnginePosition;

		int CalculateMaximum(
			const Settings* const settings, 
			const int windowSizeIncrement
		) const;
	};

	struct VolumeLabel
	{
		using SlideBar = VolumeSlideBar;
		
		VolumeLabel(
			const Sprite* parent,
			const SlideBar* const slideBar,
			const Theme* const theme,
			const float* const parentOpacity,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			const Font* const font,
			TextTextureLoader* const textureLoader
		);

		float GetTopEdgePosition() const;

		void UpdateOpacity();
		void Update();
		void Draw() const;

	private:
		static string FormatValue(const float value);

		string CalculateMessage() const;
		
		static const float c_FontSize;
		static const char* c_BeginningOfMessage;
		static const Vector2f c_EnginePosition;

		float m_MostRecentValue = c_DeactivatedTracker;

		const float* m_ParentOpacity = nullptr;
		const SlideBar* m_SlideBar = nullptr;

		unique_ptr<TextLineSprite> m_Sprite = nullptr;
	};

	struct WindowSizeLabel 
	{
		using SlideBar = WindowSizeSlideBar;
		
		WindowSizeLabel(
			const Sprite* parent,
			const SlideBar* const slideBar,
			const Theme* const theme,
			const float* const parentOpacity,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConstants,
			const Font* const font,
			TextTextureLoader* const textureLoader
		);

		float GetTopEdgePosition() const;

		void UpdateOpacity();
		void Update();
		void Draw() const;

	private:
		static string FormatValue(const float value);
		
		string CalculateMessage() const;
		
		static const float c_FontSize;
		static const char* c_BeginningOfMessage;
		static const Vector2f c_EnginePosition;

		int m_MostRecentValue = c_DeactivatedTracker;

		const float* m_ParentOpacity = nullptr;
		const SlideBar* m_SlideBar = nullptr;

		unique_ptr<TextLineSprite> m_Sprite = nullptr;
	};

	struct SettingsMenuAdjuster
	{
		virtual float CalculateHeight() const = 0;
		virtual Vector2f CalculateDimensions() const = 0;

		virtual void UpdateOpacity() = 0;
		virtual void Update() = 0;
		virtual void Draw() const = 0;

		virtual bool HasUnsavedChanges() const = 0;

		virtual ~SettingsMenuAdjuster()
		{

		}
	};

	struct VolumeAdjuster : SettingsMenuAdjuster
	{
		using SlideBar = VolumeSlideBar;
		using Label = VolumeLabel;
		
		VolumeAdjuster(
			Settings* const settings,
			const float* parentOpacity,
			const Theme* const theme,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			TextTextureLoader* const textTextureLoader,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
			const Font* const font
		);

		float GetValue() const;

		bool HasUnsavedChanges() const override;
		float CalculateHeight() const override;
		Vector2f CalculateDimensions() const override;

		void OnApply(const float newValue);
		void UpdateOpacity() override;
		void Update() override;
		void Draw() const override;

	private:
		float m_UnappliedValue = 0;
		
		const float* const m_ParentOpacity;

		unique_ptr<SlideBar> m_SlideBar = nullptr;
		unique_ptr<Label> m_Label = nullptr;
	};

	struct WindowSizeAdjuster : SettingsMenuAdjuster
	{
		using SlideBar = WindowSizeSlideBar;
		using Label = WindowSizeLabel;
		
		WindowSizeAdjuster(
			Settings* const settings,
			const int windowSizeIncrement,
			const float* const parentOpacity,
			const Theme* const theme,
			const Sprite* const parent,
			const CoordinateTransformer* const coordTransformer,
			TextTextureLoader* const textTextureLoader,
			const InputManager* const inputManager,
			const ProgramConstants* const programConstants,
			const Font* const font
		);

		int GetValue() const;

		bool HasUnsavedChanges() const override;
		float CalculateHeight() const override;
		Vector2f CalculateDimensions() const override;

		void OnApply(const int newValue);
		void UpdateOpacity() override;
		void Update() override;
		void Draw() const override;

	private:
		int m_UnappliedValue = 0;
		
		const float* m_ParentOpacity = nullptr;

		unique_ptr<Empty> m_Empty = nullptr;
		unique_ptr<SlideBar> m_SlideBar = nullptr;
		unique_ptr<Label> m_Label = nullptr;
	};

	struct SaveButton : public Button
	{
		SaveButton(
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
            ImageTextureLoader* const imageTextureLoader,
            const Callback& saveCallback,
            const Sprite* const parent,
			const Vector2f bottomRightCorner,
			const float* const parentOpacity = nullptr
		);

	private:
		static const int c_ButtonIndex;

		static const char* const c_UnselectedTexturePath;
		static const char* const c_SelectedTexturePath;
		static const char* const c_ClickedTexturePath;
	};

	struct SettingsMenu
	{
		using ConfirmationDialogue = SettingsMenuConfirmationDialogue;
		using Adjuster = SettingsMenuAdjuster;

		SettingsMenu(
			const int windowSizeIncrement,
			const bool* const programIsMuted,
			const CoordinateTransformer* const coordTransformer,
			const InputManager* const inputManager,
            const ProgramConstants* const programConstants,
			const Font* const font,
            ImageTextureLoader* const imageTextureLoader,
			TextTextureLoader* const textTextureLoader,
			Settings* const settings,
			const Callback& muteUnmuteCallback,
            const Callback& exitCallback,
            const CameraEmpty* const cameraEmpty
		);

		void Update();
		void Draw() const;

	private:
		void Apply();
		void ExitSafely();
		void OnCancellingConfirmationDialogue();

		void FadeIn();
		void FadeOut();

		bool IsFadingOut() const;
		bool IsFadingIn() const;

		bool HasUnsavedChanges() const;

		static const float c_MaxFadeOutTick;
		static const float c_MaxFadeInTick;

		Callback m_ExitCallback;
		float m_Opacity = 1;
		float m_FadeOutTick = c_DeactivatedTick;
		float m_FadeInTick = c_DeactivatedTick;
	
		Settings* m_Settings = nullptr;
		const ProgramConstants* m_ProgramConstants = nullptr;

		unique_ptr<VolumeAdjuster> m_VolumeAdjuster = nullptr;
		unique_ptr<WindowSizeAdjuster> m_WindowSizeAdjuster = nullptr;

		unique_ptr<Button> m_MuteButton = nullptr;
		unique_ptr<Button> m_TopRightExitButton = nullptr;
		unique_ptr<Button> m_CenterSaveButton = nullptr;
		unique_ptr<Button> m_CenterExitButton = nullptr;

		vector<Adjuster*> m_Adjusters = {};
		vector<Button*> m_Buttons = {};

		unique_ptr<Empty> m_Empty = nullptr;
		unique_ptr<ThemedBacking> m_Backing = nullptr;
		unique_ptr<ConfirmationDialogue> m_ConfirmationDialogue = nullptr;
	};
}
