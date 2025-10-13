#include "GUI.h"
#include "Enums.h"
#include "Graphics.h"
#include "Player.h"
#include "ProgramConstants.h"
#include "Utils.h"

#include "raylib.h"

#include <cmath>
#include <format>
#include <memory>
#include <stdexcept>

namespace BlastOff
{
	const char* const GUIBar::c_BackingTexturePath = "ui/barBacking.png";

	const float GUIBar::c_Height = 3 / 5.0f;
	const float GUIBar::c_CornerMargin = 1 / 10.0f;
	const float GUIBar::c_BetweenMargin = GUIBar::c_CornerMargin * 2;
	const float GUIBar::c_SmoothingPeriod = 1 / 6.0f;

	const Vector2f GUIBar::c_EnergySizeMultiplier =
	{
		0.9656020f,
		0.8390805f
	};

	const Sprite* GUIBar::GetBackingSprite() const
	{
		return m_BackingSprite.get();
	}

	float GUIBar::GetSmoothStatisticValue() const
	{
		return m_SmoothStatisticValue;
	}

	Vector2f GUIBar::GetEnergySpriteSize() const
	{
		return m_EnergySprite->GetEngineSize();
	}

	void GUIBar::Update()
	{
		const auto updateSmoothStatisticValue =
			[this]()
			{
				if (m_IsFirstUpdateCall)
				{
					m_SmoothStatisticValue = *m_StatisticValue;
					m_IsFirstUpdateCall = false;
				}

				const int targetFramerate =
				{
					m_ProgramConstants->GetTargetFramerate()
				};
				const float smoothingFrames = c_SmoothingPeriod * targetFramerate;

				const float sf = m_SmoothStatisticValue;
				const float fr = smoothingFrames;
				m_SmoothStatisticValue =
				{
					(sf * (fr - 1) / fr) + ((*m_StatisticValue) / fr)
				};
			};

		const auto updateEnergyCrop =
			[this]()
			{
				const Crop2f crop = { .right = 1 - m_SmoothStatisticValue };
				m_EnergySprite->SetCrop(crop);
			};

		UpdateStatistic();
		updateSmoothStatisticValue();
		updateEnergyCrop();
	}

	void GUIBar::Draw() const
	{
		m_BackingSprite->Draw();
		m_EnergySprite->Draw();
	}

	GUIBar::GUIBar(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const float* const statisticValue,
		const char* const energyTexturePath
	) :
		m_CameraEmpty(cameraEmpty),
		m_ProgramConstants(programConstants),
		m_StatisticValue(statisticValue)
	{
		const auto initializeBackingSprite =
			[&, this]()
			{
				const Texture* backingTexture = 
				{
					imageTextureLoader->LazyLoadTexture(c_BackingTexturePath)
				};
				m_BackingSprite = std::make_unique<ImageSprite>(
					coordTransformer,
					programConstants,
					backingTexture
				);
				m_BackingSprite->SetParent(m_CameraEmpty);
			};

		const auto initializeEnergySprite =
			[&, this]()
			{
				const Texture* texture =
				{
					imageTextureLoader->LazyLoadTexture(energyTexturePath)
				};
				m_EnergySprite = std::make_unique<ImageSprite>(
					coordTransformer,
					programConstants,
					texture
				);
				m_EnergySprite->SetParent(m_BackingSprite.get());
			};

		const auto initializeBackingEngineRect =
			[&, this]()
			{
				const Vector2i spriteSize = m_BackingSprite->GetImageSize();
				const float aspectRatio = spriteSize.x / (float)spriteSize.y;
				const Vector2f viewportSize =
				{
					coordTransformer->GetViewportSize()
				};

				const float xMult = aspectRatio;
				const float margin = c_CornerMargin;
				const Vector2f enginePosition =
				{
					(((c_Height * xMult) - viewportSize.x) / 2.0f) + margin,
					((c_Height - viewportSize.y) / 2.0f) + margin
				};
				const Vector2f engineSize =
				{
					c_Height * aspectRatio,
					c_Height
				};
				const Rect2f engineRect(enginePosition, engineSize);
				m_BackingSprite->SetEngineRect(engineRect);
			};

		const auto initializeEnergyEngineRect =
			[this]()
			{
				constexpr Vector2f enginePosition = Vector2f::Zero();
				const Vector2f backingSize =
				{
					m_BackingSprite->CalculateRealSize()
				};
				const Vector2f engineSize =
				{
					backingSize * c_EnergySizeMultiplier
				};

				const Rect2f engineRect(enginePosition, engineSize);
				m_EnergySprite->SetEngineRect(engineRect);
			};

		initializeBackingSprite();
		initializeEnergySprite();
		initializeBackingEngineRect();
		initializeEnergyEngineRect();
	}


	FuelBar::FuelBar(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const Player* const player
	) :
		GUIBar(
			coordTransformer,
			programConstants,
			imageTextureLoader,
			cameraEmpty,
			&m_StatisticValue,
			c_EnergyTexturePath
		),
		m_Player(player)
	{

	}

	const char* const FuelBar::c_EnergyTexturePath =
	{
		"ui/fuelBarEnergy.png"
	};

	void FuelBar::UpdateStatistic()
	{
		m_StatisticValue = m_Player->GetFuelRatio();
	}


	SpeedupBar::SpeedupBar(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const Player* const player
	) :
		GUIBar(
			coordTransformer,
			programConstants,
			imageTextureLoader,
			cameraEmpty,
			&m_StatisticValue,
			c_EnergyTexturePath
		),
		m_Player(player)
	{
		const auto moveUp =
			[this]()
			{
				const Vector2f translation = { 0, c_Height + c_BetweenMargin };
				m_BackingSprite->Move(translation);
			};

		moveUp();
	}

	const char* const SpeedupBar::c_EnergyTexturePath =
	{
		"ui/speedBarEnergy.png"
	};

	void SpeedupBar::UpdateStatistic()
	{
		m_StatisticValue = m_Player->GetSpeedUpRatio();
	}


	void GUILabel::Update()
	{
		m_Sprite->Update();
	}

	void GUILabel::Draw() const
	{
		m_Sprite->Draw();
	}

	GUILabel::GUILabel(
		const Vector2f enginePosition,
		const Colour4i colour,
		const float fontSize,
		const char* const message,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	)
	{
		m_Sprite = std::make_unique<TextLineSprite>(
			enginePosition,
			colour,
			fontSize,
			coordTransformer,
			programConstants,
			textTextureLoader,
			font,
			message
		);
	}


	BarLabelComponent::BarLabelComponent(
		const GUIBar* bar,
		const Type type,
		const Colour4i colour,
		const char* const message,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	) :
		GUILabel(
			c_Offset,
			colour,
			c_FontSize,
			message,
			coordTransformer,
			programConstants,
			textTextureLoader,
			font
		),
		m_Type(type),
		m_Bar(bar)
	{
		GUILabel::Update();

		const auto setParent =
			[&, this]()
			{
				const Sprite* newParent = m_Bar->GetBackingSprite();
				m_Sprite->SetParent(newParent);
			};

		setParent();
	}

	void BarLabelComponent::Update()
	{
		struct CropRange
		{
			float start = 0, end = 0;

			float Apply(const float original) const
			{
				const float unboundedResult =
				{
					(original - start) / (end - start)
				};

				if (unboundedResult > 1)
					return 1;
				else if (unboundedResult < 0)
					return 0;
				else
					return unboundedResult;
			}
		};

		const auto calculateCropRange =
			[this]() -> CropRange
			{
				const Vector2f energySize = m_Bar->GetEnergySpriteSize();
				const Vector2f engineSize = m_Sprite->GetEngineSize();

				const float breadth = engineSize.x / energySize.x;
				return
				{
					.start = (1 - breadth) / 2.0f,
					.end = (1 + breadth) / 2.0f
				};
			};

		const auto calculateCrop =
			[&, this]() -> Crop2f
			{
				const float smoothStatistic = m_Bar->GetSmoothStatisticValue();

				float leftCrop = 0, rightCrop = 0;
				if (m_Type == Type::AboveEnergy)
					rightCrop = 1 - smoothStatistic;
				else if (m_Type == Type::AboveBacking)
					leftCrop = smoothStatistic;
				else
				{
					const char* const message =
					{
						"BarLabel::Update()::calculateCrop failed: "
						"m_Type enum has an invalid value."
					};
					throw std::runtime_error(message);
				}

				// this sprite is smaller than the energy sprite, 
				// so we need to convert the crop to fit in its size
				using Range = CropRange;
				const Range range = calculateCropRange();

				leftCrop = range.Apply(leftCrop);
				rightCrop = range.Apply(rightCrop);

				return { .left = leftCrop, .right = rightCrop };
			};

		const auto updateCrop =
			[&, this]()
			{
				const Crop2f newCrop = calculateCrop();
				m_Sprite->SetCrop(newCrop);
			};

		updateCrop();
	}

	const float BarLabelComponent::c_FontSize = 48;
	const Vector2f BarLabelComponent::c_Offset = { 0, 0 };


	BarLabel::BarLabel(
		const GUIBar* bar,
		const Colours colours,
		const char* const message,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	)
	{
		using Type = BarLabelType;
		m_BackingLabel = std::make_unique<BarLabelComponent>(
			bar,
			Type::AboveBacking,
			colours.aboveBacking,
			message,
			coordTransformer,
			programConstants,
			textTextureLoader,
			font
		);
		m_EnergyLabel = std::make_unique<BarLabelComponent>(
			bar,
			Type::AboveEnergy,
			colours.aboveEnergy,
			message,
			coordTransformer,
			programConstants,
			textTextureLoader,
			font
		);
	}

	void BarLabel::Update()
	{
		m_BackingLabel->Update();
		m_EnergyLabel->Update();
	}

	void BarLabel::Draw() const
	{
		m_BackingLabel->Draw();
		m_EnergyLabel->Draw();
	}


	const char* const FuelBarLabel::c_Message = "Fuel";
	const BarLabelColours FuelBarLabel::c_Colours =
	{
		.aboveBacking = Colour4i(0x20),
		.aboveEnergy = Colour4i(0x00, 0x40, 0x00)
	};

	FuelBarLabel::FuelBarLabel(
		const GUIBar* fuelBar,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	) :
		BarLabel(
			fuelBar,
			c_Colours,
			c_Message,
			coordTransformer,
			programConstants,
			textTextureLoader,
			font
		)
	{

	}


	const char* const SpeedupBarLabel::c_Message = "Speedup";
	const BarLabelColours SpeedupBarLabel::c_Colours =
	{
		.aboveBacking = Colour4i(0x20),
		.aboveEnergy = Colour4i(0x00, 0x40, 0x40)
	};


	SpeedupBarLabel::SpeedupBarLabel(
		const GUIBar* speedupBar,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	) :
		BarLabel(
			speedupBar,
			c_Colours,
			c_Message,
			coordTransformer,
			programConstants,
			textTextureLoader,
			font
		)
	{

	}


	void Button::Translate(const Vector2f translation)
	{
		m_Sprite->Move(translation);
	}

	void Button::SetParent(const Sprite* const parent)
	{
		m_Sprite->SetParent(parent);
	}

	void Button::UseUnselectedTexture()
	{
		m_Sprite->SetTexture(m_UnselectedTexture);
	}

	void Button::Enable()
	{
		m_IsEnabled = true;
	}

	void Button::Disable()
	{
		m_IsEnabled = false;
	}

	void Button::UpdateOpacity()
	{
		if (m_ParentOpacity)
			m_Sprite->SetOpacity(*m_ParentOpacity);
	}

	void Button::Update()
	{
		const auto updateSelection =
			[this]()
			{
				const Vector2f mousePosition =
				{
					m_InputManager->CalculateMousePosition()
				};
				const Rect2f engineRect = m_Sprite->CalculateRealRect();

				m_IsSelected = engineRect.CollideWithPoint(mousePosition);

				const bool leftButtonDown =
				{
					m_InputManager->GetMouseButtonDown(MOUSE_BUTTON_LEFT)
				};
				m_ShouldShowClickedSprite = leftButtonDown && m_IsSelected;
			};

		const auto checkForClick =
			[this]()
			{
				const bool mousePressed = 
				{
					m_InputManager->GetMouseButtonPressed(MOUSE_BUTTON_LEFT)
				};
				if (mousePressed && !m_HasJustEnabled)
					m_ClickCallback();
			};

		const auto updateSprite =
			[this]()
			{
				if (m_ShouldShowClickedSprite)
					m_Sprite->SetTexture(m_ClickedTexture);
				else if (m_IsSelected)
					m_Sprite->SetTexture(m_SelectedTexture);
				else
					m_Sprite->SetTexture(m_UnselectedTexture);

				m_Sprite->Update();
			};

		UseUnselectedTexture();
		UpdateOpacity();
		
		if (!m_IsEnabled)
			return;

		updateSelection();
		if (m_IsSelected)
			checkForClick();
		updateSprite();

		m_HasJustEnabled = false;
	}

	void Button::Draw() const
	{
		m_Sprite->Draw();
	}

	Button::Button(
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
		const float* const parentOpacity 
	) :
		m_ClickCallback(clickCallback),
		m_CoordTransformer(coordTransformer),
		m_InputManager(inputManager),
		m_ParentOpacity(parentOpacity)
	{
		const auto lazyLoadTextures =
			[&, this]()
			{
				m_UnselectedTexture =
				{
					imageTextureLoader->LazyLoadTexture(unselectedTexturePath)
				};
				m_SelectedTexture =
				{
					imageTextureLoader->LazyLoadTexture(selectedTexturePath)
				};
				m_ClickedTexture =
				{
					imageTextureLoader->LazyLoadTexture(clickedTexturePath)
				};
			};

		const auto initializeBacking =
			[&, this]()
			{
				const Rect2f engineRect(enginePosition, engineSize);
				m_Sprite = std::make_unique<ImageSprite>(
					engineRect,
					coordTransformer,
					programConstants,
					m_UnselectedTexture
				);
				m_Sprite->SetParent(parent);
			};

		lazyLoadTextures();
		initializeBacking();
	}


	YesButton::YesButton(
		const Callback& yesCallback,
		ImageTextureLoader* const imageTextureLoader,
		const Vector2f bottomRightCorner,
		const Sprite* const parent,
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants
	) :
		Button(
			yesCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			CenterMenuButton::c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConstants
		)
	{
		const auto initializePosition = 
			[&, this]()
			{
				const Vector2f offset1 = CenterMenuButton::c_AdditionalOffset;
				const Vector2f offset2 = 
				{
					CenterMenuButton::CalculateOffsetByIndex(c_ButtonIndex)
				};
				const Vector2f position = bottomRightCorner + offset1 + offset2;
				m_Sprite->Move(position);
			};

		initializePosition();
	}

	const int YesButton::c_ButtonIndex = 0;

	const char* const YesButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/yes.png"
	};
	const char* const YesButton::c_SelectedTexturePath =
	{
		"ui/button/selected/yes.png"
	};
	const char* const YesButton::c_ClickedTexturePath = 
	{
		"ui/button/clicked/yes.png"
	};


	SettingsMenuCancelButton::SettingsMenuCancelButton(
		const Callback& cancelCallback,
		ImageTextureLoader* const imageTextureLoader,
		const Vector2f bottomRightCorner,
		const Sprite* const parent,
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants
	) :
		Button(
			cancelCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			CenterMenuButton::c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConstants
		)
	{
		const auto initializePosition = 
			[&, this]()
			{
				const Vector2f offset1 = CenterMenuButton::c_AdditionalOffset;
				const Vector2f offset2 = 
				{
					CenterMenuButton::CalculateOffsetByIndex(c_ButtonIndex)
				};
				const Vector2f position = bottomRightCorner + offset1 + offset2;
				m_Sprite->Move(position);
			};

		initializePosition();
	}

	const int SettingsMenuCancelButton::c_ButtonIndex = 1;

	const char* const SettingsMenuCancelButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/cancel.png"
	};
	const char* const SettingsMenuCancelButton::c_SelectedTexturePath = 
	{
		"ui/button/selected/cancel.png"
	};
	const char* const SettingsMenuCancelButton::c_ClickedTexturePath = 
	{
		"ui/button/clicked/cancel.png"
	};

	
	NoButton::NoButton(
		const Callback& noCallback,
		ImageTextureLoader* const imageTextureLoader,
		const Vector2f bottomRightCorner,
		const Sprite* const parent,
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants
	) :
		Button(
			noCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			CenterMenuButton::c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConstants
		)
	{
		const auto initializePosition = 
			[&, this]()
			{
				const Vector2f offset1 = CenterMenuButton::c_AdditionalOffset;
				const Vector2f offset2 = 
				{
					CenterMenuButton::CalculateOffsetByIndex(c_ButtonIndex)
				};
				const Vector2f position = bottomRightCorner + offset1 + offset2;
				m_Sprite->Move(position);
			};

		initializePosition();
	}

	const int NoButton::c_ButtonIndex = 2;

	const char* const NoButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/no.png"
	};
	const char* const NoButton::c_SelectedTexturePath =
	{
		"ui/button/selected/no.png"
	};
	const char* const NoButton::c_ClickedTexturePath = 
	{
		"ui/button/clicked/no.png"
	};


	const Theme Theme::c_DarkTheme = 
	{
		.outerBackingRoundness = 22 / 100.0f,
		.outerBackingStrokeWidth = 2 / 44.0f,
		.outerBackingColours = 
		{ 
			.stroke = c_Black, 
			.fill = Colour4i(0x30 / 2) 
		},
		.outerMargins = { 4 / 45.0f, 4 / 45.0f },

		.innerBackingRoundness = 1 / 10.0f,
		.innerBackingStrokeWidth = 2 / 44.0f,
		.innerBackingColours = 
		{ 
			.stroke = c_Black, 
			.fill = Colour4i(0x30) 
		},
		.innerMargins = { 1 / 5.0f, 1 / 5.0f },

		.textColour = c_White
	};


	ThemedBacking::ThemedBacking(
		const Vector2f innerSize,
		const Theme* const theme,
		const Sprite* const parent,
		const float* const parentOpacity,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader
	) :
		m_ParentOpacity(parentOpacity)
	{
		const auto initializeInnerBacking = 
			[&, this]()
			{
				const Vector2f backingDimensions = 
				{
					innerSize + (theme->innerMargins * 2)
				};
				const Rect2f backingRect(Vector2f::Zero(), backingDimensions);

				m_InnerBackingFill = std::make_unique<RoundedRectangleSprite>(
					backingRect,
					theme->innerBackingColours.fill,
					theme->innerBackingRoundness,
					coordTransformer,
					programConstants
				);
				m_InnerBackingFill->SetParent(parent);

				m_InnerBackingStroke = std::make_unique<RoundedRectangleSprite>(
					backingRect,
					theme->innerBackingColours.stroke,
					theme->innerBackingRoundness,
					coordTransformer,
					programConstants,
					theme->innerBackingStrokeWidth
				);
				m_InnerBackingStroke->SetParent(m_InnerBackingFill.get());
			};

		const auto initializeOuterBacking = 
			[&, this]()
			{
				const Vector2f middleSize = 
				{
					m_InnerBackingStroke->GetEngineSize()
				};
				const Vector2f backingDimensions = 
				{
					middleSize + (theme->outerMargins * 2)
				};
				const Rect2f backingRect(Vector2f::Zero(), backingDimensions);

				m_OuterBackingFill = std::make_unique<RoundedRectangleSprite>(
					backingRect,
					theme->outerBackingColours.fill,
					theme->outerBackingRoundness,
					coordTransformer,
					programConstants
				);
				m_OuterBackingFill->SetParent(parent);

				m_OuterBackingStroke = std::make_unique<RoundedRectangleSprite>(
					backingRect,
					theme->outerBackingColours.stroke,
					theme->outerBackingRoundness,
					coordTransformer,
					programConstants,
					theme->outerBackingStrokeWidth
				);
				m_OuterBackingStroke->SetParent(m_OuterBackingFill.get());
			};

		initializeInnerBacking();
		initializeOuterBacking();
	}

	Vector2f ThemedBacking::CalculateBottomRightCorner() const
	{
		const Vector2f backingSize = 
		{
			m_OuterBackingStroke->GetEngineSize()
		};
		return (backingSize / 2.0f).InvertY();
	}

	Vector2f ThemedBacking::GetEngineSize() const
	{
		return m_OuterBackingStroke->GetEngineSize();
	}

	void ThemedBacking::UpdateOpacity()
	{
		m_OuterBackingFill->SetOpacity(*m_ParentOpacity);
		m_OuterBackingStroke->SetOpacity(*m_ParentOpacity);
		m_InnerBackingFill->SetOpacity(*m_ParentOpacity);
		m_InnerBackingStroke->SetOpacity(*m_ParentOpacity);
	}

	void ThemedBacking::Update()
	{
		UpdateOpacity();

		m_OuterBackingFill->Update();
		m_OuterBackingStroke->Update();
		m_InnerBackingFill->Update();
		m_InnerBackingStroke->Update();
	}

	void ThemedBacking::Draw() const
	{
		m_OuterBackingFill->Draw();
		m_OuterBackingStroke->Draw();
		m_InnerBackingFill->Draw();
		m_InnerBackingStroke->Draw();
	}

	
	SlideState::SlideState(
		const Vector2f startingPosition,
		const Vector2f endingPosition,
		const float maxTick,
		Sprite* sprite,
		const ProgramConstants* const programConstants
	) :
		m_StartingPosition(startingPosition),
		m_EndingPosition(endingPosition),
		m_MaxSlideTick(maxTick),
		m_Sprite(sprite),
		m_ProgramConstants(programConstants)
	{

	}

	Vector2f SlideState::GetStartingPosition() const
	{
		return m_StartingPosition;
	}

	bool SlideState::HasJustFinished() const
	{
		return m_HasJustFinished;
	}

	void SlideState::Slide(const float waitInSeconds)
	{
		if (waitInSeconds <= 0)
			m_SlideTick = m_MaxSlideTick;
		else
			m_WaitTick = waitInSeconds;
	}

	void SlideState::SwapPositions()
	{
		const Vector2f temp = m_StartingPosition;
		m_StartingPosition = m_EndingPosition;
		m_EndingPosition = temp;
	}

	void SlideState::Update()
	{
		const auto updateSliding =
			[this]()
			{
				float progress = 
				{
					1 - (m_SlideTick / m_MaxSlideTick)
				};
				progress = DoubleSineInterpolation(progress);

				const Vector2f enginePosition = Lerp(
					m_StartingPosition, 
					m_EndingPosition, 
					progress
				);
				m_Sprite->SetLocalPosition(enginePosition);

				const float targetFrametime =
				{
					m_ProgramConstants->GetTargetFrametime()
				};
				m_SlideTick -= targetFrametime;
			};

		const auto updateWaiting = 
			[this]()
			{
				m_WaitTick -= m_ProgramConstants->GetTargetFrametime();
				if (!IsWaiting())
					Slide();
			};

		m_HasJustFinished = false;

		if (IsWaiting())
			updateWaiting();

		if (IsSliding())
		{
			updateSliding();
			if (!IsSliding())
				m_HasJustFinished = true;
		}
	}

	bool SlideState::IsWaiting() const
	{
		return m_WaitTick >= 0;
	}

	bool SlideState::IsSliding() const
	{
		return m_SlideTick >= 0;
    }


	BackgroundTint::BackgroundTint(
		const Sprite* const parent,
		const Colour4i colour,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		const Direction slideDirection,
		const float slideLength
	)
	{
		const auto initializeSprite = 
			[&, this]()
			{
				const Rect2f engineRect(
					Vector2f::Zero(), 
					coordTransformer->GetViewportSize()
				);
				m_Sprite = std::make_unique<RectangleSprite>(
					engineRect,
					colour,
					coordTransformer,
					programConstants
				);
				m_Sprite->SetParent(parent);
			};

		const auto initializeSlideState = 
			[&, this]()
			{
				const Vector2f viewportSize = 
				{
					coordTransformer->GetViewportSize()
				};
				const Vector2f directionVector = 
				{
					DirectionToVector2f(Direction::Down)
				};
				const Vector2f startPosition = 
				{
					viewportSize * (-directionVector)
				};
				constexpr Vector2f endPosition = Vector2f::Zero();
				m_SlideState = std::make_unique<SlideState>(
					startPosition,
					endPosition,
					slideLength,
					m_Sprite.get(),
					programConstants
				);
				m_SlideState->Slide(c_SlideInWait);
			};

		initializeSprite();
		initializeSlideState();
	}

	void BackgroundTint::Enable()
	{
		m_IsEnabled = true;
	}

	void BackgroundTint::Update()
	{
		if (!m_IsEnabled)
			return;

		m_Sprite->Update();
		m_SlideState->Update();
	}

	void BackgroundTint::Draw() const
	{
		if (!m_IsEnabled)
			return;

		m_Sprite->Draw();
	}

	const float BackgroundTint::c_SlideInWait = 0;


	bool ConfirmationDialogue::IsEnabled() const
	{
		return m_IsEnabled;
	}

	void ConfirmationDialogue::Enable()
	{
		m_IsEnabled = true;
	}

	void ConfirmationDialogue::Disable()
	{
		m_IsEnabled = false;
	}

	void ConfirmationDialogue::Update()
	{
		if (!m_IsEnabled)
			return;
		
		m_BackgroundTint->Update();
		m_Backing->Update();
		m_Message->Update();
		m_YesButton->Update();
		m_CancelButton->Update();
		m_NoButton->Update();
	}

	void ConfirmationDialogue::Draw() const
	{
		if (!m_IsEnabled)
			return;
		
		m_BackgroundTint->Draw();
		m_Backing->Draw();
		m_Message->Draw();
		m_YesButton->Draw();
		m_CancelButton->Draw();
		m_NoButton->Draw();
	}

	const float ConfirmationDialogue::c_FontSize = 32;
	const float ConfirmationDialogue::c_LineSpacing = 3 / 2.0f;

	const Colour4i ConfirmationDialogue::c_BackgroundTintColour = 
	{ 
		0, 0, 0, 0x40 
	};
	const Direction ConfirmationDialogue::c_BackgroundTintSlideDirection = 
	{
		Direction::Down
	};
	const float ConfirmationDialogue::c_BackgroundTintSlideLength = 1 / 4.0f;

	ConfirmationDialogue::ConfirmationDialogue(
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
	)
	{
		const auto initializeEmpty = 
			[&, this]()
			{
				m_Empty = std::make_unique<Empty>(
					enginePosition,
					coordTransformer,
					programConstants
				);
				m_Empty->SetParent(parent);
			};

		const auto initializeMessage = 
			[&, this]()
			{
				constexpr Vector2f enginePosition = Vector2f::Zero();
				m_Message = std::make_unique<TextSprite>(
					enginePosition,
					theme->textColour,
					c_FontSize,
					c_LineSpacing,
					coordTransformer,
					programConstants,
					textTextureLoader,
					font,
					message,
					m_Empty.get()
				);
			};

		const auto initializeBacking = 
			[&, this]()
			{
				const Vector2f messageSize = m_Message->CalculateEngineSize();
				m_Backing = std::make_unique<ThemedBacking>(
					messageSize,
					theme,
					m_Empty.get(),
					&c_Opacity,
					coordTransformer,
					programConstants,
					imageTextureLoader
				);
			};

		const auto initializeBackgroundTint = 
			[&, this]()
			{
				m_BackgroundTint = std::make_unique<BackgroundTint>(
					parent,
					c_BackgroundTintColour,
					coordTransformer,
					programConstants,
					c_BackgroundTintSlideDirection,
					c_BackgroundTintSlideLength
				);
				m_BackgroundTint->Enable();
			};

		const auto initializeButtons = 
			[&, this]()
			{
				const Vector2f bottomRightCorner = 
				{
					m_Backing->CalculateBottomRightCorner()
				};
				m_YesButton = std::make_unique<YesButton>(
					yesCallback,
					imageTextureLoader,
					bottomRightCorner,
					m_Empty.get(),
					coordTransformer,
					inputManager,
					programConstants
				);
				m_CancelButton = std::make_unique<SettingsMenuCancelButton>(
					cancelCallback,
					imageTextureLoader,
					bottomRightCorner,
					m_Empty.get(),
					coordTransformer,
					inputManager,
					programConstants
				);
				m_NoButton = std::make_unique<NoButton>(
					noCallback,
					imageTextureLoader,
					bottomRightCorner,
					m_Empty.get(),
					coordTransformer,
					inputManager,
					programConstants
				);
			};

		initializeEmpty();
		initializeMessage();
		initializeBacking();
		initializeBackgroundTint();
		initializeButtons();
	}


	const Vector2f TopRightButton::c_EngineSize = { 1 / 2.0f, 1 / 2.0f };
	const Vector2f TopRightButton::c_Margins = { 1 / 10.0f, 1 / 10.0f };

	const Vector2f CenterMenuButton::c_EngineSize = { 1 / 2.0f, 1 / 2.0f };
	const Vector2f CenterMenuButton::c_Margins = { 1 / 10.0f, 1 / 10.0f };
	const Vector2f CenterMenuButton::c_AdditionalOffset = 
	{ 
		- 3 / 20.0f, -3 / 30.0f
	};

	const Vector2f MainMenuButton::c_EngineSize = { 5 / 4.0f, 5 / 4.0f };
	const Vector2f MainMenuButton::c_Margins = { 1 / 10.0f, 1 / 10.0f };

	
    MuteButton::MuteButton(
        const bool* const programIsMuted,
        const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
        const ProgramConstants* const programConstants,
        ImageTextureLoader* const imageTextureLoader,
        const Callback& muteUnmuteCallback,
        const Sprite* const parent
    ) :
		Button(
			muteUnmuteCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			TopRightButton::c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConstants
		),
		m_IsActive(programIsMuted)
    {
        const auto initializePosition = 
            [&, this]()
        {
			const Vector2f engineSize = TopRightButton::c_EngineSize;
			const Vector2f margins = TopRightButton::c_Margins;

            const Vector2f viewportSize = coordTransformer->GetViewportSize();
            Vector2f enginePosition =
            {
                ((viewportSize - engineSize) / 2.0f) - margins
            };
            const Vector2f indexOffset = 
			{
				TopRightButton::CalculateOffsetByIndex(c_ButtonIndex)
			};
			enginePosition -= indexOffset;
			Translate(enginePosition);
        };

        const auto initializeActiveBar = 
            [&, this]()
        {
            const Rect2f engineRect(Vector2f::Zero(), c_ActiveBarSize);
            m_ActiveBar = ImageSprite::LoadFromPath(
                engineRect,
                c_ActiveBarTexturePath,
                coordTransformer,
                programConstants,
                imageTextureLoader
            );
            m_ActiveBar->SetParent(m_Sprite.get());
        };

        initializePosition();
        initializeActiveBar();
    }

    void MuteButton::Update()
    {
        Button::Update();
    }

    void MuteButton::Draw() const
    {
        Button::Draw();

        if (*m_IsActive)
            m_ActiveBar->Draw();
    }

	const int MuteButton::c_ButtonIndex = 0;

    const Vector2f MuteButton::c_ActiveBarSize = { 21 / 40.0f, 21 / 40.0f };

    const char* const MuteButton::c_UnselectedTexturePath =
    {
        "ui/button/unselected/mute.png"
    };
    const char* const MuteButton::c_SelectedTexturePath =
    {
        "ui/button/selected/mute.png"
    };
	const char* const MuteButton::c_ClickedTexturePath =
	{
		"ui/button/clicked/mute.png"
	};
    const char* const MuteButton::c_ActiveBarTexturePath =
    {
        "ui/button/activeBar.png"
    };


	ResetButton::ResetButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& resetCallback,
		const Vector2f enginePosition,
		const Sprite* const parent
	) :
		Button(
			resetCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			enginePosition,
			TopRightButton::c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConstants
		)
	{

	}

	void ResetButton::SetParent(const Sprite* const parent)
	{
		m_Sprite->SetParent(parent);
	}

	const char* const ResetButton::c_UnselectedTexturePath =
	{
		"ui/button/unselected/reset.png"
	};
	const char* const ResetButton::c_SelectedTexturePath =
	{
		"ui/button/selected/reset.png"
	};
	const char* const ResetButton::c_ClickedTexturePath =
	{
		"ui/button/clicked/reset.png"
	};


	TopRightResetButton::TopRightResetButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& resetCallback,
		const CameraEmpty* const cameraEmpty
	) :
		ResetButton(
			coordTransformer,
			inputManager,
			programConstants,
			imageTextureLoader,
			resetCallback,
			Vector2f::Zero(),
			cameraEmpty
		)
	{
		const auto initializePosition = 
			[&, this]()
			{
				const Vector2f engineSize = TopRightButton::c_EngineSize;
				const Vector2f margins = TopRightButton::c_Margins;

				const Vector2f viewportSize = coordTransformer->GetViewportSize();
				Vector2f enginePosition =
				{
					((viewportSize - engineSize) / 2.0f) - margins
				};
				const Vector2f indexOffset = 
				{
					TopRightButton::CalculateOffsetByIndex(c_ButtonIndex)
				};
				enginePosition -= indexOffset;
				Translate(enginePosition);
			};

		const auto initializeSlideState = 
			[&, this]()
			{
				const Vector2f engineSize = TopRightButton::c_EngineSize;
				const Vector2f margins = TopRightButton::c_Margins;

				const float xOffset = engineSize.x + margins.x;
				const Vector2f slideOffset = { xOffset, 0 };

				const Vector2f startingPosition = m_Sprite->GetLocalPosition();
				const Vector2f endingPosition = startingPosition + slideOffset;

				m_SlideState = std::make_unique<SlideState>(
					startingPosition,
					endingPosition,
					c_MaxSlideOutTick,
					m_Sprite.get(), 
					programConstants
				);
			};

		initializePosition();
		initializeSlideState();
	}

	void TopRightResetButton::SlideOut()
	{
		m_SlideState->Slide(c_SlideOutWait);
	}

	void TopRightResetButton::Update()
	{
		ResetButton::Update();

		m_SlideState->Update();
	}

	const float TopRightResetButton::c_SlideOutWait = 1 / 12.0f;
	const float TopRightResetButton::c_MaxSlideOutTick = 1 / 4.0f;
	const int TopRightResetButton::c_ButtonIndex = 1;


	TopRightExitButton::TopRightExitButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& exitCallback,
		const Sprite* const parent,
		const ProgramState menuType
	) :
		ExitButton(
			exitCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			TopRightButton::c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConstants
		)
	{
		const auto calculateButtonIndex = 
			[&, this]()
			{
				switch (menuType)
				{
					case ProgramState::Game:
						return c_ButtonIndexInGame;

					case ProgramState::SettingsMenu:
						return c_ButtonIndexInSettingsMenu;

					default:
						throw std::runtime_error(
							"Unable to construct TopRightExitButton: "
							"Invalid parameter \"menuType\"."
						);
				}
			};

		const auto initializePosition = 
			[&, this]()
			{				
				const Vector2f engineSize = TopRightButton::c_EngineSize;
				const Vector2f margins = TopRightButton::c_Margins;

				const Vector2f viewportSize = 
				{
					coordTransformer->GetViewportSize()
				};
				Vector2f enginePosition =
				{
					((viewportSize - engineSize) / 2.0f) - margins
				};

				const int buttonIndex = calculateButtonIndex();
				const Vector2f indexOffset = 
				{
					TopRightButton::CalculateOffsetByIndex(buttonIndex)
				};
				enginePosition -= indexOffset;
				Translate(enginePosition);
			};

		const auto initializeSlideState = 
			[&, this]()
			{
				const Vector2f engineSize = TopRightButton::c_EngineSize;
				const Vector2f margins = TopRightButton::c_Margins;

				const float xOffset = engineSize.x + margins.x;
				const Vector2f slideOffset = { xOffset, 0 };

				const Vector2f startingPosition = m_Sprite->GetLocalPosition();
				const Vector2f endingPosition = startingPosition + slideOffset;

				m_SlideState = std::make_unique<SlideState>(
					startingPosition,
					endingPosition,
					c_MaxSlideOutTick,
					m_Sprite.get(), 
					programConstants
				);
			};

		initializePosition();
		initializeSlideState();
	}

	void TopRightExitButton::SlideOut()
	{
		m_SlideState->Slide(c_SlideOutWait);
	}

	void TopRightExitButton::Update()
	{
		ExitButton::Update();

		m_SlideState->Update();
	}

	const int TopRightExitButton::c_ButtonIndexInGame = 2;
	const int TopRightExitButton::c_ButtonIndexInSettingsMenu = 1;
	const float TopRightExitButton::c_SlideOutWait = 0;
	const float TopRightExitButton::c_MaxSlideOutTick = 1 / 4.0f;

	const char* const TopRightExitButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/smallExit.png"
	};
	const char* const TopRightExitButton::c_SelectedTexturePath = 
	{
		"ui/button/selected/smallExit.png"
	};
	const char* const TopRightExitButton::c_ClickedTexturePath = 
	{
		"ui/button/clicked/smallExit.png"
	};


	PlayButton::PlayButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& playCallback,
		const CameraEmpty* const cameraEmpty
	) :
		Button(
			playCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			MainMenuButton::c_EngineSize,
			cameraEmpty,
			coordTransformer,
			inputManager,
			programConstants
		)
	{
		const Vector2f engineSize = MainMenuButton::c_EngineSize;
		const Vector2f margins = MainMenuButton::c_Margins;

		const Vector2f translation = (engineSize + margins) / 2.0f;
		Translate(translation.InvertX());
	}

	const char* const PlayButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/play.png"
	};
	const char* const PlayButton::c_SelectedTexturePath = 
	{
		"ui/button/selected/play.png"
	};
	const char* const PlayButton::c_ClickedTexturePath =
	{
		"ui/button/clicked/play.png"
	};


	SettingsButton::SettingsButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& settingsCallback,
		const CameraEmpty* const cameraEmpty
	) :
		Button(
			settingsCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			MainMenuButton::c_EngineSize,
			cameraEmpty,
			coordTransformer,
			inputManager,
			programConstants
		)
	{
		const Vector2f engineSize = MainMenuButton::c_EngineSize;
		const Vector2f margins = MainMenuButton::c_Margins;

		const Vector2f translation = (engineSize + margins) / 2.0f;
		Translate(translation);
	}

	const char* const SettingsButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/settings.png"
	};
	const char* const SettingsButton::c_SelectedTexturePath = 
	{
		"ui/button/selected/settings.png"
	};
	const char* const SettingsButton::c_ClickedTexturePath =
	{
		"ui/button/clicked/settings.png"
	};


	MainMenuExitButton::MainMenuExitButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& exitCallback,
		const CameraEmpty* const cameraEmpty		
	) :
		ExitButton(
			exitCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			MainMenuButton::c_EngineSize,
			cameraEmpty,
			coordTransformer,
			inputManager,
			programConstants
		)
	{
		const Vector2f engineSize = MainMenuButton::c_EngineSize;
		const Vector2f margins = MainMenuButton::c_Margins;

		const float engineY = -(engineSize.y + margins.y) / 2.0f;
		const Vector2f enginePosition = { 0, engineY };
		Translate(enginePosition);
	}

	const char* const MainMenuExitButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/largeExit.png"
	};
	const char* const MainMenuExitButton::c_SelectedTexturePath = 
	{
		"ui/button/selected/largeExit.png"
	};
	const char* const MainMenuExitButton::c_ClickedTexturePath =
	{
		"ui/button/clicked/largeExit.png"
	};


	GameEndMenu::GameEndMenu(
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
	) 
	{
		const auto initializeEmpty = 
			[&, this]()
			{
				constexpr Vector2f enginePosition = Vector2f::Zero();
				m_Empty = std::make_unique<Empty>(
					enginePosition,
					coordTransformer,
					programConstants
				);
				m_Empty->SetParent(cameraEmpty);
			};

		const auto initializeBackingFill =
			[&, this]()
			{
				constexpr Rect2f defaultEngineRect(Vector2f::Zero(), { 1, 1 });
				m_BackingFill = std::make_unique<RoundedRectangleSprite>(
					defaultEngineRect,
					backingColour,
					c_Roundness,
					coordTransformer,
					programConstants
				);
				m_BackingFill->SetParent(m_Empty.get());
			};

		const auto initializeBackingStroke =
			[&, this]()
			{
				constexpr Rect2f defaultEngineRect(Vector2f::Zero(), { 1, 1 });
				m_BackingStroke = std::make_unique<RoundedRectangleSprite>(
					defaultEngineRect,
					c_Black,
					c_Roundness,
					coordTransformer,
					programConstants,
					c_StrokeWidth
				);
				m_BackingStroke->SetParent(m_Empty.get());
			};

		const auto initializeMainMessage =
			[&, this]()
			{
				m_Message = std::make_unique<TextLineSprite>(
					Vector2f::Zero(),
					c_Black,
					c_MessageFontSize,
					coordTransformer,
					programConstants,
					textTextureLoader,
					font,
					messageText
				);
				m_Message->SetParent(m_Empty.get());
			};

		const auto calculateSlideInStartPosition =
			[&, this]() -> Vector2f
			{
				const Vector2f viewportSize =
				{
					coordTransformer->GetViewportSize()
				};
				const Vector2f engineSize = m_BackingFill->GetEngineSize();

				const float xPosition =
				{
					-(viewportSize.x + engineSize.x) / 2.0f
				};
				return { xPosition, 0 };
			};

		const auto initializeSlideState = 
			[&, this]()
			{
				const Vector2f startPosition = calculateSlideInStartPosition();
				constexpr Vector2f endPosition = Vector2f::Zero();
				m_SlideState = std::make_unique<SlideState>(
					startPosition,
					endPosition,
					c_MaxSlideInTick,
					m_Empty.get(),
					programConstants
				);
			};

		const auto updateEngineSize =
			[this]()
			{
				m_Message->Update();
				const Vector2f messageSize = m_Message->GetEngineSize();
				const Vector2f newSize = messageSize + c_Margins;

				m_BackingFill->SetEngineSize(newSize);
				m_BackingStroke->SetEngineSize(newSize);
			};

		const auto initializeCenterButtons =
			[&, this]()
			{
				const Vector2f backingSize = m_BackingStroke->GetEngineSize();
				const Vector2f bottomRightCorner = 
				{
					(backingSize / 2.0f).InvertY()
				};
				m_ResetButton = std::make_unique<CenterMenuResetButton>(
					coordTransformer,
					inputManager,
					programConstants,
					imageTextureLoader,
					resetCallback,
					m_Empty.get(),
					bottomRightCorner
				);
				m_ExitButton = std::make_unique<CenterMenuExitButton>(
					coordTransformer,
					inputManager,
					programConstants,
					imageTextureLoader,
					exitCallback,
					m_Empty.get(),
					bottomRightCorner
				);
			};

		initializeEmpty();
		initializeBackingFill();
		initializeBackingStroke();
		initializeMainMessage();
		initializeSlideState();
		updateEngineSize();
		initializeCenterButtons();
	}

	void GameEndMenu::Enable()
	{
		m_IsEnabled = true;
		m_SlideState->Slide(c_SlideInWait);
	}

	void GameEndMenu::Update()
	{
		if (m_IsEnabled)
		{
			m_Message->Update();
			m_ResetButton->Update();
			m_ExitButton->Update();
			m_SlideState->Update();
		}
	}

	void GameEndMenu::Draw() const
	{
		if (m_IsEnabled)
		{
			m_BackingFill->Draw();
			m_BackingStroke->Draw();
			m_Message->Draw();
			m_ResetButton->Draw();
			m_ExitButton->Draw();
		}
	}

	const float GameEndMenu::c_Roundness = 1 / 4.0f;
	const float GameEndMenu::c_StrokeWidth = 2 / 44.0f;
	const float GameEndMenu::c_MessageFontSize = 96;
	const float GameEndMenu::c_SlideInWait = 0;
	const float GameEndMenu::c_MaxSlideInTick = 1 / 4.0f;

	const Vector2f GameEndMenu::c_Margins = { 3 / 10.0f, 1 / 10.0f };
	const Vector2f GameEndMenu::c_ResetButtonAdditionalOffset =
	{
		- 3 / 20.0f, -3 / 30.0f
	};


	WinMenu::WinMenu(
		const Callback& resetCallback,
		const Callback& exitCallback,
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		TextTextureLoader* const textTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const Font* const font
	) :
		GameEndMenu(
			c_BackingColour,
			c_MessageText,
			resetCallback,
			exitCallback,
			coordTransformer,
			inputManager,
			programConstants,
			imageTextureLoader,
			textTextureLoader,
			cameraEmpty,
			font
		)
	{

	}

	const Colour4i WinMenu::c_BackingColour = Colour4i(0x80, 0xFF, 0x80);
	const char* const WinMenu::c_MessageText = "You won!";


	LoseMenu::LoseMenu(
		const Callback& resetCallback,
		const Callback& exitCallback,
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		TextTextureLoader* const textTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const Font* const font
	) :
		GameEndMenu(
			c_BackingColour,
			c_MessageText,
			resetCallback,
			exitCallback,
			coordTransformer,
			inputManager,
			programConstants,
			imageTextureLoader,
			textTextureLoader,
			cameraEmpty,
			font
		)
	{

	}

	const Colour4i LoseMenu::c_BackingColour = Colour4i(0xFF, 0x80, 0x80);
	const char* const LoseMenu::c_MessageText = "Loser!";


    MainMenu::MainMenu(
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
    ) :
        m_ProgramConstants(programConstants),
        m_CoordTransformer(coordTransformer),
		m_InputManager(inputManager),
        m_CameraEmpty(cameraEmpty),
        m_ImageTextureLoader(imageTextureLoader),
        m_TextTextureLoader(textTextureLoader),
        m_Font(font),
        m_WindowPosition(windowPosition),
        m_WindowSize(windowSize)
    {
        const auto initializeButtons = 
            [&, this]()
            {
                m_SettingsButton = std::make_unique<SettingsButton>(
                    m_CoordTransformer,
                    m_InputManager,
                    m_ProgramConstants,
                    m_ImageTextureLoader,
                    settingsCallback,
                    m_CameraEmpty
                );
                m_PlayButton = std::make_unique<PlayButton>(
                    m_CoordTransformer,
                    m_InputManager,
                    m_ProgramConstants,
                    m_ImageTextureLoader,
                    playCallback,
                    m_CameraEmpty
                );
                m_ExitButton = std::make_unique<MainMenuExitButton>(
                    m_CoordTransformer,
                    m_InputManager,
                    m_ProgramConstants,
                    m_ImageTextureLoader,
                    exitCallback,
                    m_CameraEmpty
                );
            };

        initializeButtons();
    }
    
    void MainMenu::Update()
    {
        m_SettingsButton->Update();
        m_PlayButton->Update();
        m_ExitButton->Update();
    }

    void MainMenu::Draw() const
    {
        m_SettingsButton->Draw();
        m_PlayButton->Draw();
        m_ExitButton->Draw();
    }
	

	VolumeSlideBar::VolumeSlideBar(
		Settings* const settings,
		const float* const parentOpacity,
		const Sprite* const parent,
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants
	) :
		SettingsMenuSlideBar<float>(
			c_EnginePosition,
			parentOpacity,
			GetMasterVolume(),
			c_Minimum,
			c_Maximum,
			parent,
			settings,
			coordTransformer,
			inputManager,
			programConstants
		)
	{

	}

	const float VolumeSlideBar::c_Minimum = 0;
	const float VolumeSlideBar::c_Maximum = 1;
	const Vector2f VolumeSlideBar::c_EnginePosition = Vector2f::Zero();

	
	WindowSizeSlideBar::WindowSizeSlideBar(
		Settings* const settings,
		const int windowSizeIncrement,
		const float* const parentOpacity,
		const Sprite* const parent,
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants
	) :
		SettingsMenuSlideBar<int>(
			c_EnginePosition,
			parentOpacity,
			settings->GetWindowSize().y,
			c_Minimum,
			CalculateMaximum(settings, windowSizeIncrement),
			parent,
			settings,
			coordTransformer,
			inputManager,
			programConstants,
			windowSizeIncrement
		)
	{

	}

	const int WindowSizeSlideBar::c_Minimum = 420;
	
	const Vector2f WindowSizeSlideBar::c_EnginePosition = Vector2f::Zero();

	int WindowSizeSlideBar::CalculateMaximum(
		const Settings* const settings,
		const int windowSizeIncrement
	) const
	{
		// the user can select up to 90% of the current screen height
		const Vector2i screenSize = settings->GetScreenSize();
		const float unfloored = screenSize.y * 9 / 10.0f;
		const float unrounded = FloorToFraction(
			unfloored, 
			windowSizeIncrement
		);
		return (int)roundf(unrounded);
	}

	
	VolumeLabel::VolumeLabel(
		const Sprite* parent,
		const SlideBar* const slideBar,
		const Theme* const theme,
		const float* const parentOpacity,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		const Font* const font,
		TextTextureLoader* const textureLoader
	) :
		m_SlideBar(slideBar),
		m_ParentOpacity(parentOpacity)
	{
		m_Sprite = std::make_unique<TextLineSprite>(
			c_EnginePosition,
			theme->textColour,
			c_FontSize,
			coordTransformer,
			programConstants,
			textureLoader,
			font
		);
		m_Sprite->SetParent(parent);
	}

	float VolumeLabel::GetTopEdgePosition() const
	{
		return m_Sprite->GetEdgePosition(Direction::Up);
	}

	void VolumeLabel::UpdateOpacity()
	{
		m_Sprite->SetOpacity(*m_ParentOpacity);
	}

	void VolumeLabel::Update() 
	{
		const auto updateMessage = 
			[this]()
			{
				const float value = m_SlideBar->GetValue();
				if (value != m_MostRecentValue)
				{
					const string message = CalculateMessage();
					m_Sprite->SetMessage(message);

					m_MostRecentValue = value;
				}
			};

		updateMessage();

		m_Sprite->SetOpacity(*m_ParentOpacity);
		m_Sprite->Update();
	}

	void VolumeLabel::Draw() const
	{
		m_Sprite->Draw();
	}

	string VolumeLabel::FormatValue(const float value) 
	{
		const int rounded = (int)roundf(value * 100);
		return std::format("{}%", rounded);
	}

	string VolumeLabel::CalculateMessage() const
	{
		const float windowSize = m_SlideBar->GetValue();
		const string formattedValue = FormatValue(windowSize);
		return std::format("{}: {}", c_BeginningOfMessage, formattedValue);
	}

	const float VolumeLabel::c_FontSize = 32;
	const char* VolumeLabel::c_BeginningOfMessage = "Audio Volume";
	const Vector2f VolumeLabel::c_EnginePosition = { 0, 2 / 5.0f };


	WindowSizeLabel::WindowSizeLabel(
		const Sprite* parent,
		const SlideBar* const slideBar,
		const Theme* const theme,
		const float* const parentOpacity,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		const Font* const font,
		TextTextureLoader* const textureLoader
	) :
		m_SlideBar(slideBar),
		m_ParentOpacity(parentOpacity)
	{
		m_Sprite = std::make_unique<TextLineSprite>(
			c_EnginePosition,
			theme->textColour,
			c_FontSize,
			coordTransformer,
			programConstants,
			textureLoader,
			font
		);
		m_Sprite->SetParent(parent);
	}

	float WindowSizeLabel::GetTopEdgePosition() const
	{
		return m_Sprite->GetEdgePosition(Direction::Up);
	}

	void WindowSizeLabel::UpdateOpacity()
	{
		m_Sprite->SetOpacity(*m_ParentOpacity);
	}

	void WindowSizeLabel::Update() 
	{
		const auto updateMessage = 
			[this]()
			{
				const int value = m_SlideBar->GetValue();
				if (value != m_MostRecentValue)
				{
					const string message = CalculateMessage();
					m_Sprite->SetMessage(message);

					m_MostRecentValue = value;
				}
			};

		updateMessage();

		m_Sprite->SetOpacity(*m_ParentOpacity);
		m_Sprite->Update();
	}

	void WindowSizeLabel::Draw() const
	{
		m_Sprite->Draw();
	}

	string WindowSizeLabel::FormatValue(const float value)
	{
		const int rounded = (int)roundf(value);
		return std::to_string(rounded);
	}

	string WindowSizeLabel::CalculateMessage() const
	{
		const float windowSize = m_SlideBar->GetValue();
		const string formattedValue = FormatValue(windowSize);
		return std::format("{}: {}", c_BeginningOfMessage, formattedValue);
	}

	const float WindowSizeLabel::c_FontSize = 32;
	const char* WindowSizeLabel::c_BeginningOfMessage = "Window Size";
	const Vector2f WindowSizeLabel::c_EnginePosition = { 0, 2 / 5.0f };

	
	SettingsMenuConfirmationDialogue::SettingsMenuConfirmationDialogue(
		const Callback& yesCallback,
		const Callback& cancelCallback,
		const Callback& noCallback,
		const Sprite* const parent,
		const Theme* const theme,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConstants* const programConstants,
		const InputManager* const inputManager,
		const Font* const font,
		TextTextureLoader* const textTextureLoader,
		ImageTextureLoader* const imageTextureLoader
	) :
		ConfirmationDialogue(
			yesCallback,
			cancelCallback,
			noCallback,
			c_Message,
			c_EnginePosition,
			parent,
			theme,
			coordTransformer,
			programConstants,
			inputManager,
			font,
			textTextureLoader,
			imageTextureLoader
		)
	{
		const auto initializeSlideState = 
			[&, this]()
			{
				const Vector2f viewportSize = 
				{
					coordTransformer->GetViewportSize()
				};
				const Vector2f backingSize = m_Backing->GetEngineSize();
				const float startX = (-viewportSize.x / 2.0f) - backingSize.x;
				const Vector2f startPosition = { startX, 0 };
				constexpr Vector2f endPosition = Vector2f::Zero();

				m_SlideState = std::make_unique<SlideState>(
					startPosition,
					endPosition,
					c_MaxSlideInTick,
					m_Empty.get(),
					programConstants
				);
				m_OffScreenPosition = startPosition;
			};

		initializeSlideState();
	}

	const float SettingsMenuConfirmationDialogue::c_MaxSlideInTick = 1 / 4.0f;
	const float SettingsMenuConfirmationDialogue::c_SlideWait = 0;
	const char* const SettingsMenuConfirmationDialogue::c_Message = 
	{
		"You haven't saved yet!\n"
		"Would you like to save?"
	};
	const Vector2f SettingsMenuConfirmationDialogue::c_EnginePosition = 
	{
		Vector2f::Zero()
	};

	void SettingsMenuConfirmationDialogue::Enable()
	{
		ConfirmationDialogue::Enable();

		if (m_SlideState->GetStartingPosition() != m_OffScreenPosition)
			m_SlideState->SwapPositions();

		m_SlideState->Slide(c_SlideWait);
		m_IsSlidingOut = false;
	}

	void SettingsMenuConfirmationDialogue::Disable() 
	{
		if (m_SlideState->GetStartingPosition() == m_OffScreenPosition)
			m_SlideState->SwapPositions();

		m_SlideState->Slide(c_SlideWait);
		m_IsSlidingOut = true;
	}

	void SettingsMenuConfirmationDialogue::Update()
	{
		if (!m_IsEnabled)
			return;

		ConfirmationDialogue::Update();
		m_SlideState->Update();
		if (m_SlideState->HasJustFinished() && m_IsSlidingOut)
		{
			m_IsSlidingOut = false;
			m_IsEnabled = false;
		}
	}


	VolumeAdjuster::VolumeAdjuster(
		Settings* const settings,
		const float* parentOpacity,
		const Theme* const theme,
		const Sprite* const parent,
		const CoordinateTransformer* const coordTransformer,
		TextTextureLoader* const textTextureLoader,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		const Font* const font
	) :
		m_ParentOpacity(parentOpacity)
	{
		const auto initializeObjects =	
			[&, this]()
			{
				m_SlideBar = std::make_unique<VolumeSlideBar>(
					settings,
					parentOpacity,
					parent,
					coordTransformer,
					inputManager,
					programConstants
				);
				m_Label = std::make_unique<Label>(
					parent,
					m_SlideBar.get(),
					theme,
					parentOpacity,
					coordTransformer,
					programConstants,
					font,
					textTextureLoader
				);
			};

		initializeObjects();

		m_UnappliedValue = GetValue();
	}

	float VolumeAdjuster::GetValue() const
	{
		return m_SlideBar->GetValue();
	}

	bool VolumeAdjuster::HasUnsavedChanges() const
	{
		return GetValue() != m_UnappliedValue;
	}

	float VolumeAdjuster::CalculateHeight() const
	{
		return 1;
	}

	Vector2f VolumeAdjuster::CalculateDimensions() const
	{
		return { m_SlideBar->GetWidth(), CalculateHeight() };
	}

	void VolumeAdjuster::OnApply(const float newValue)
	{
		m_UnappliedValue = newValue;
	}

	void VolumeAdjuster::UpdateOpacity()
	{
		// todo
	}

	void VolumeAdjuster::Update()
	{
		m_SlideBar->Update();
		m_Label->Update();
	}

	void VolumeAdjuster::Draw() const
	{
		m_SlideBar->Draw();
		m_Label->Draw();
	}


	WindowSizeAdjuster::WindowSizeAdjuster(
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
	) :
		m_ParentOpacity(parentOpacity)
	{
		const auto initializeObjects = 
			[&, this]()
			{
				m_Empty = std::make_unique<Empty>(
					Vector2f::Zero(),
					coordTransformer,
					programConstants
				);
				m_Empty->SetParent(parent);

				m_SlideBar = std::make_unique<SlideBar>(
					settings,
					windowSizeIncrement,
					parentOpacity,
					m_Empty.get(),
					coordTransformer,
					inputManager,
					programConstants
				);
				m_Label = std::make_unique<Label>(
					m_Empty.get(),
					m_SlideBar.get(),
					theme,
					parentOpacity,
					coordTransformer,
					programConstants,
					font,
					textTextureLoader
				);
			};

		const auto updatePosition = 
			[&, this]()
			{
				const float height = CalculateHeight();
				m_Empty->SetLocalPosition({ 0, -height / 4.0f });
			};

		initializeObjects();
		updatePosition();
		
		m_UnappliedValue = GetValue();
	}
	
	int WindowSizeAdjuster::GetValue() const
	{
		return m_SlideBar->GetValue();
	}

	bool WindowSizeAdjuster::HasUnsavedChanges() const
	{
		return GetValue() != m_UnappliedValue;
	}

	float WindowSizeAdjuster::CalculateHeight() const
	{
		const float bottom = m_SlideBar->GetBottomEdgePosition();
		const float top = m_Label->GetTopEdgePosition();

		return top - bottom;
	}

	Vector2f WindowSizeAdjuster::CalculateDimensions() const
	{
		return { m_SlideBar->GetWidth(), CalculateHeight() };
	}

	void WindowSizeAdjuster::OnApply(const int newValue) 
	{
		m_UnappliedValue = newValue;
	}

	void WindowSizeAdjuster::UpdateOpacity()
	{
		m_Empty->SetOpacity(*m_ParentOpacity);
		m_SlideBar->UpdateOpacity();
		m_Label->UpdateOpacity();
	}

	void WindowSizeAdjuster::Update()
	{
		m_Empty->Update();
		m_SlideBar->Update();
		m_Label->Update();
	}

	void WindowSizeAdjuster::Draw() const
	{
		m_SlideBar->Draw();
		m_Label->Draw();
	}


	SaveButton::SaveButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& saveCallback,
		const Sprite* const parent,
		const Vector2f bottomRightCorner,
		const float* const parentOpacity
	) : 
		Button(
			saveCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			CenterMenuButton::c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConstants,
			parentOpacity
		)
	{
		const auto initializePosition = 
			[&, this]()
			{
				const Vector2f offset1 = CenterMenuButton::c_AdditionalOffset;
				const Vector2f offset2 = 
				{
					CenterMenuButton::CalculateOffsetByIndex(c_ButtonIndex)
				};
				const Vector2f position = bottomRightCorner + offset1 + offset2;
				m_Sprite->Move(position);
			};

		initializePosition();
	}

	const int SaveButton::c_ButtonIndex = 0;

	const char* const SaveButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/save.png"
	};
	const char* const SaveButton::c_SelectedTexturePath = 
	{
		"ui/button/selected/save.png"
	};
	const char* const SaveButton::c_ClickedTexturePath = 
	{
		"ui/button/clicked/save.png"
	};


	CenterMenuResetButton::CenterMenuResetButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& resetCallback,
		const Sprite* const parent,
		const Vector2f bottomRightCorner
	) :
		ResetButton(
			coordTransformer,
			inputManager,
			programConstants,
			imageTextureLoader,
			resetCallback,
			Vector2f::Zero(),
			parent
		)
	{
		const auto initializePosition = 
			[&, this]()
			{
				const Vector2f offset1 = CenterMenuButton::c_AdditionalOffset;
				const Vector2f offset2 = 
				{
					CenterMenuButton::CalculateOffsetByIndex(c_ButtonIndex)
				};
				const Vector2f position = bottomRightCorner + offset1 + offset2;
				m_Sprite->Move(position);
			};

		initializePosition();
	}

	const int CenterMenuResetButton::c_ButtonIndex = 0;


	CenterMenuExitButton::CenterMenuExitButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConstants* const programConstants,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& exitCallback,
		const Sprite* const parent,
		const Vector2f bottomRightCorner,
		const float* const parentOpacity
	) :
		Button(
			exitCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			CenterMenuButton::c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConstants,
			parentOpacity
		)
	{
		const auto initializePosition = 
			[&, this]()
			{
				const Vector2f offset1 = CenterMenuButton::c_AdditionalOffset;
				const Vector2f offset2 = 
				{
					CenterMenuButton::CalculateOffsetByIndex(c_ButtonIndex)
				};
				const Vector2f position = bottomRightCorner + offset1 + offset2;
				m_Sprite->Move(position);
			};

		initializePosition();
	}

	const int CenterMenuExitButton::c_ButtonIndex = 1;

	const char* const CenterMenuExitButton::c_UnselectedTexturePath = 
	{
		"ui/button/unselected/smallExit.png"
	};
	const char* const CenterMenuExitButton::c_SelectedTexturePath = 
	{
		"ui/button/selected/smallExit.png"
	};
	const char* const CenterMenuExitButton::c_ClickedTexturePath = 
	{
		"ui/button/clicked/smallExit.png"
	};


	SettingsMenu::SettingsMenu(
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
	) :
		m_Settings(settings),
		m_ExitCallback(exitCallback),
		m_ProgramConstants(programConstants)
	{
		const auto initializeEmpty = 	
			[&, this]()
			{
				constexpr Vector2f enginePosition = Vector2f::Zero();
				m_Empty = std::make_unique<Empty>(
					enginePosition,
					coordTransformer,
					programConstants
				);
				m_Empty->SetParent(cameraEmpty);
			};

		const auto safeExitCallback = 
			[this]()
			{
				ExitSafely();
			};

		const auto initializeTopRightButtons = 
			[&, this]()
			{
				m_MuteButton = std::make_unique<MuteButton>(
					programIsMuted,
					coordTransformer,
					inputManager,
					programConstants,
					imageTextureLoader,
					muteUnmuteCallback,
					m_Empty.get()
				);
				m_TopRightExitButton = std::make_unique<TopRightExitButton>(
					coordTransformer,
					inputManager,
					programConstants,
					imageTextureLoader,
					safeExitCallback,
					m_Empty.get(),
					ProgramState::SettingsMenu
				);
			};

		const auto initializeAdjusters = 
			[&, this]()
			{
				m_VolumeAdjuster = std::make_unique<VolumeAdjuster>(
					m_Settings,
					&m_Opacity,
					&Theme::c_DarkTheme,
					m_Empty.get(),
					coordTransformer,
					textTextureLoader,
					inputManager,
					programConstants,
					font
				);
				m_WindowSizeAdjuster = std::make_unique<WindowSizeAdjuster>(
					m_Settings,
					windowSizeIncrement,
					&m_Opacity,
					&Theme::c_DarkTheme,
					m_Empty.get(),
					coordTransformer,
					textTextureLoader,
					inputManager,
					programConstants,
					font
				);
			};

		const auto calculateSizeOfComponents = 
			[this]()
			{
				// width = max width of every element
				// height = sum of all components' sizes + margin
				const array componentSizes = 
				{
					m_WindowSizeAdjuster->CalculateDimensions()
				};
				const Theme& backingTheme = Theme::c_DarkTheme;
				const float startHeight = 
				{
					backingTheme.outerMargins.y * (componentSizes.size() - 1)
				};
				Vector2f result = { 0, startHeight };
				for (const Vector2f size : componentSizes)
				{
					result.x = std::max(result.x, size.x);
					result.y += size.y;
				}
				return result;
			};

		const auto initializeBacking = 
			[&, this]()
			{
				const Vector2f innerSize = calculateSizeOfComponents();
				m_Backing = std::make_unique<ThemedBacking>(
					innerSize,
					&Theme::c_DarkTheme,
					m_Empty.get(),
					&m_Opacity,
					coordTransformer,
					programConstants,
					imageTextureLoader
				);
			};

		const auto applyCallback = 
			[this]()
			{
				Apply();
			};

		const auto initializeCenterButtons = 
			[&, this]()
			{
				const Vector2f bottomRightCorner = m_Backing->CalculateBottomRightCorner();

				m_CenterSaveButton = std::make_unique<SaveButton>(
					coordTransformer,
					inputManager,
					programConstants,
					imageTextureLoader,
					applyCallback,
					m_Empty.get(),
					bottomRightCorner,
					&m_Opacity
				);
				m_CenterExitButton = std::make_unique<CenterMenuExitButton>(
					coordTransformer,
					inputManager,
					programConstants,
					imageTextureLoader,
					safeExitCallback,
					m_Empty.get(),
					bottomRightCorner,
					&m_Opacity
				);
			};

		const auto yesCallback = 
			[this]()
			{
				Apply();
				m_ExitCallback();
			};

		const auto cancelCallback = 
			[this]()
			{
				OnCancellingConfirmationDialogue();
			};

		const auto noCallback = 
			[this]()
			{
				m_ExitCallback();
			};

		const auto initializeConfirmationDialogue = 
			[&, this]()
			{
				m_ConfirmationDialogue = std::make_unique<ConfirmationDialogue>(
					yesCallback,
					cancelCallback,
					noCallback,
					m_Empty.get(),
					&Theme::c_DarkTheme,
					coordTransformer,
					programConstants,
					inputManager,
					font,
					textTextureLoader,
					imageTextureLoader
				);
			};

		const auto initializeLists = 	
			[this]()
			{
				m_Adjusters = 
				{ 
					m_VolumeAdjuster.get(),
					m_WindowSizeAdjuster.get()
				};
				m_Buttons = 
				{
					m_MuteButton.get(),
					m_TopRightExitButton.get(),
					m_CenterSaveButton.get(),
					m_CenterExitButton.get()
				};
			};

		initializeEmpty();
		initializeTopRightButtons();
		initializeAdjusters();
		initializeBacking();
		initializeCenterButtons();
		initializeConfirmationDialogue();
		initializeLists();
	}

	void SettingsMenu::Update()
	{
		const auto fadeIn = 
			[this]()
			{
				const float progress = 1 - (m_FadeInTick / c_MaxFadeInTick);
				m_Opacity = progress;

				m_FadeInTick -= m_ProgramConstants->GetTargetFrametime();
			};

		const auto fadeOut = 
			[this]()
			{
				const float progress = m_FadeOutTick / c_MaxFadeOutTick;
				m_Opacity = progress;

				m_FadeOutTick -= m_ProgramConstants->GetTargetFrametime();
			};

		if (IsFadingIn())
			fadeIn();
		else if (IsFadingOut())
			fadeOut();

		for (Button* button : m_Buttons)
			button->Update();

		if (m_ConfirmationDialogue->IsEnabled())
		{
			m_ConfirmationDialogue->Update();
			
			m_Backing->UpdateOpacity();

			// for (Adjuster* adjuster : m_Adjusters)
			// 	adjuster->UpdateOpacity();
			m_VolumeAdjuster->UpdateOpacity();
		}
		else
		{
			m_Empty->Update();
			m_Backing->Update();
			
			// for (Adjuster* adjuster : m_Adjusters)
			// 	adjuster->Update();
			m_VolumeAdjuster->Update();

			m_ConfirmationDialogue->Update();
		}
	}

	void SettingsMenu::Draw() const
	{
		m_Backing->Draw();
		
		// for (const Adjuster* adjuster : m_Adjusters)
		// 	adjuster->Draw();
		m_VolumeAdjuster->Draw();
	
		for (const Button* button : m_Buttons)
			button->Draw();

		m_ConfirmationDialogue->Draw();
	}

	void SettingsMenu::Apply()
	{
		const int windowHeight = m_WindowSizeAdjuster->GetValue();
		m_Settings->ChangeWindowHeight(windowHeight);
		m_WindowSizeAdjuster->OnApply(windowHeight);
	}

	void SettingsMenu::ExitSafely()
	{
		if (HasUnsavedChanges())
		{
			m_ConfirmationDialogue->Enable();
			FadeOut();

			for (Button* button : m_Buttons)
				button->Disable();
		}
		else
			m_ExitCallback();
	}

	void SettingsMenu::OnCancellingConfirmationDialogue()
	{
		m_ConfirmationDialogue->Disable();
		FadeIn();

		for (Button* button : m_Buttons)
			button->Enable();
	}

	void SettingsMenu::FadeIn() 
	{
		m_FadeInTick = c_MaxFadeInTick;
	}

	void SettingsMenu::FadeOut() 
	{
		m_FadeOutTick = c_MaxFadeOutTick;
	}

	bool SettingsMenu::IsFadingIn() const
	{
		return m_FadeInTick >= 0;
	}

	bool SettingsMenu::IsFadingOut() const
	{
		return m_FadeOutTick >= 0;
	}

	bool SettingsMenu::HasUnsavedChanges() const
	{
		for (const Adjuster* const adjuster : m_Adjusters)
			if (adjuster->HasUnsavedChanges())
				return true;

		return false;
	}

	const float SettingsMenu::c_MaxFadeInTick = 1 / 4.0f;
	const float SettingsMenu::c_MaxFadeOutTick = 1 / 4.0f;
}
