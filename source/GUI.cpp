#include "GUI.h"
#include "Graphics.h"
#include "Player.h"
#include "Utils.h"
#include "raylib.h"

namespace BlastOff
{
	const char* const GUIBar::c_BackingTexturePath = "ui/BarBacking.png";

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
					m_ProgramConfig->GetTargetFramerate()
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
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const float* const statisticValue,
		const char* const energyTexturePath
	) :
		m_CameraEmpty(cameraEmpty),
		m_ProgramConfig(programConfig),
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
					programConfig,
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
					programConfig,
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
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const Player* const player
	) :
		GUIBar(
			coordTransformer,
			programConfig,
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
		"ui/FuelBarEnergy.png"
	};

	void FuelBar::UpdateStatistic()
	{
		m_StatisticValue = m_Player->GetFuelRatio();
	}


	SpeedupBar::SpeedupBar(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const Player* const player
	) :
		GUIBar(
			coordTransformer,
			programConfig,
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
		"ui/SpeedBarEnergy.png"
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
		const ProgramConfiguration* const programConfig,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	)
	{
		m_Sprite = std::make_unique<TextSprite>(
			enginePosition,
			colour,
			fontSize,
			message,
			coordTransformer,
			programConfig,
			textTextureLoader,
			font
		);
	}


	BarLabelComponent::BarLabelComponent(
		const GUIBar* bar,
		const Type type,
		const Colour4i colour,
		const char* const message,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	) :
		GUILabel(
			c_Offset,
			colour,
			c_FontSize,
			message,
			coordTransformer,
			programConfig,
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
		const ProgramConfiguration* const programConfig,
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
			programConfig,
			textTextureLoader,
			font
		);
		m_EnergyLabel = std::make_unique<BarLabelComponent>(
			bar,
			Type::AboveEnergy,
			colours.aboveEnergy,
			message,
			coordTransformer,
			programConfig,
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
		const ProgramConfiguration* const programConfig,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	) :
		BarLabel(
			fuelBar,
			c_Colours,
			c_Message,
			coordTransformer,
			programConfig,
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
		const ProgramConfiguration* const programConfig,
		TextTextureLoader* const textTextureLoader,
		const Font* const font
	) :
		BarLabel(
			speedupBar,
			c_Colours,
			c_Message,
			coordTransformer,
			programConfig,
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
			};

		const auto checkForClick =
			[this]()
			{
				if (m_InputManager->GetMouseButtonPressed(MOUSE_BUTTON_LEFT))
				{
					m_ClickCallback();
				}

				m_ShouldShowClickedSprite =
				{
					m_InputManager->GetMouseButtonDown(MOUSE_BUTTON_LEFT)
				};
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

		if (!m_IsEnabled)
			return;

		updateSelection();
		if (m_IsSelected)
			checkForClick();
		updateSprite();
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
		const ProgramConfiguration* const programConfig
	) :
		m_ClickCallback(clickCallback),
		m_CoordTransformer(coordTransformer),
		m_InputManager(inputManager)
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
					programConfig,
					m_UnselectedTexture
				);
				m_Sprite->SetParent(parent);
			};

		lazyLoadTextures();
		initializeBacking();
	}


    MuteButton::MuteButton(
        const bool* const programIsMuted,
        const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
        const ProgramConfiguration* const programConfig,
        ImageTextureLoader* const imageTextureLoader,
        const Callback& muteCallback,
        const CameraEmpty* const cameraEmpty,
		const Vector2f margins
    ) :
		Button(
			muteCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			c_EngineSize,
			cameraEmpty,
			coordTransformer,
			inputManager,
			programConfig
		),
		m_IsActive(programIsMuted),
		m_Margins(margins)
    {
        const auto initializePosition = 
            [&, this]()
        {
            const Vector2f viewportSize = coordTransformer->GetViewportSize();
            Vector2f enginePosition =
            {
                ((viewportSize - c_EngineSize) / 2.0f) - margins
            };
            const Vector2f indexOffset = TopRightButton::CalculateIndexOffset(
				c_ButtonIndex,
				m_Margins,
				c_EngineSize
			);
			enginePosition -= indexOffset;
			m_Sprite->Move(enginePosition);
        };

        const auto initializeActiveBar = 
            [&, this]()
        {
            const Rect2f engineRect(Vector2f::Zero(), c_ActiveBarSize);
            m_ActiveBar = ImageSprite::LoadFromPath(
                engineRect,
                c_ActiveBarTexturePath,
                coordTransformer,
                programConfig,
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

    const Vector2f MuteButton::c_EngineSize = { 1 / 2.0f, 1 / 2.0f };
    const Vector2f MuteButton::c_ActiveBarSize = { 21 / 40.0f, 21 / 40.0f };

    const char* const MuteButton::c_UnselectedTexturePath =
    {
        "ui/button/UnselectedMute.png"
    };
    const char* const MuteButton::c_SelectedTexturePath =
    {
        "ui/button/SelectedMute.png"
    };
	const char* const MuteButton::c_ClickedTexturePath =
	{
		"ui/button/ClickedMute.png"
	};
    const char* const MuteButton::c_ActiveBarTexturePath =
    {
        "ui/button/MuteActiveBar.png"
    };


	ResetButton::ResetButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConfiguration* const programConfig,
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
			c_EngineSize,
			parent,
			coordTransformer,
			inputManager,
			programConfig
		)
	{

	}

	void ResetButton::SetParent(const Sprite* const parent)
	{
		m_Sprite->SetParent(parent);
	}

	const Vector2f ResetButton::c_EngineSize = { 1 / 2.0f, 1 / 2.0f };

	const char* const ResetButton::c_UnselectedTexturePath =
	{
		"ui/button/UnselectedReset.png"
	};
	const char* const ResetButton::c_SelectedTexturePath =
	{
		"ui/button/SelectedReset.png"
	};
	const char* const ResetButton::c_ClickedTexturePath =
	{
		"ui/button/ClickedReset.png"
	};


	TopRightResetButton::TopRightResetButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& resetCallback,
		const CameraEmpty* const cameraEmpty,
		const Vector2f margins
	) :
		ResetButton(
			coordTransformer,
			inputManager,
			programConfig,
			imageTextureLoader,
			resetCallback,
			Vector2f::Zero(),
			cameraEmpty
		),
		m_ProgramConfig(programConfig),
		m_Margins(margins)
	{
		const Vector2f viewportSize = coordTransformer->GetViewportSize();
		Vector2f enginePosition =
		{
			((viewportSize - c_EngineSize) / 2.0f) - m_Margins
		};
		const Vector2f indexOffset = TopRightButton::CalculateIndexOffset(
			c_ButtonIndex,
			m_Margins,
			c_EngineSize
		);
		enginePosition -= indexOffset;
		m_Sprite->Move(enginePosition);
	}

	void TopRightResetButton::Update()
	{
		const auto updateSlidingOut =
			[this]()
			{
				float progress = 
				{
					1 - (m_SlideOutTick / c_MaxSlideOutTick)
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
					m_ProgramConfig->GetTargetFrametime()
				};
				m_SlideOutTick -= targetFrametime;
			};

		ResetButton::Update();

		if (IsSlidingOut())
			updateSlidingOut();
	}

	const int TopRightResetButton::c_ButtonIndex = 1;
	const float TopRightResetButton::c_MaxSlideOutTick = 1 / 4.0f;

	void TopRightResetButton::SlideOut()
	{
		m_SlideOutTick = c_MaxSlideOutTick;

		const float xOffset = c_EngineSize.x + m_Margins.x;
		const Vector2f slideOffset = { xOffset, 0 };

		m_StartingPosition = m_Sprite->GetLocalPosition();
		m_EndingPosition = m_StartingPosition + slideOffset;
	}

	bool TopRightResetButton::IsSlidingOut() const
	{
		return m_SlideOutTick >= 0;
    }


	TopRightExitButton::TopRightExitButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& exitCallback,
		const CameraEmpty* const cameraEmpty,
		const Vector2f margins
	) :
		ExitButton(
			exitCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			c_EngineSize,
			cameraEmpty,
			coordTransformer,
			inputManager,
			programConfig
		),
		m_Margins(margins)
	{
		const Vector2f viewportSize = coordTransformer->GetViewportSize();
		Vector2f enginePosition =
		{
			((viewportSize - c_EngineSize) / 2.0f) - m_Margins
		};
		const Vector2f indexOffset = TopRightButton::CalculateIndexOffset(
			c_ButtonIndex,
			m_Margins,
			c_EngineSize
		);
		enginePosition -= indexOffset;
		m_Sprite->Move(enginePosition);
	}

	const int TopRightExitButton::c_ButtonIndex = 2;
	const Vector2f TopRightExitButton::c_EngineSize = { 1 / 2.0f, 1 / 2.0f };

	const char* const TopRightExitButton::c_UnselectedTexturePath = 
	{
		"ui/button/UnselectedSmallExit.png"
	};
	const char* const TopRightExitButton::c_SelectedTexturePath = 
	{
		"ui/button/SelectedSmallExit.png"
	};
	const char* const TopRightExitButton::c_ClickedTexturePath = 
	{
		"ui/button/ClickedSmallExit.png"
	};


	PlayButton::PlayButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& playCallback,
		const CameraEmpty* const cameraEmpty,
		const Vector2f margins
	) :
		Button(
			playCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			c_EngineSize,
			cameraEmpty,
			coordTransformer,
			inputManager,
			programConfig
		)
	{
		const float engineX = -(c_EngineSize.x + margins.x) / 2.0f;
		Translate({ engineX, 0 });
	}

	const Vector2f PlayButton::c_EngineSize = { 5 / 4.0f, 5 / 4.0f };

	const char* const PlayButton::c_UnselectedTexturePath = 
	{
		"ui/button/UnselectedPlay.png"
	};
	const char* const PlayButton::c_SelectedTexturePath = 
	{
		"ui/button/SelectedPlay.png"
	};
	const char* const PlayButton::c_ClickedTexturePath =
	{
		"ui/button/ClickedPlay.png"
	};


	SettingsButton::SettingsButton(
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		const Callback& settingsCallback,
		const CameraEmpty* const cameraEmpty,
		const Vector2f margins
	) :
		Button(
			settingsCallback,
			c_UnselectedTexturePath,
			c_SelectedTexturePath,
			c_ClickedTexturePath,
			imageTextureLoader,
			Vector2f::Zero(),
			c_EngineSize,
			cameraEmpty,
			coordTransformer,
			inputManager,
			programConfig
		)
	{
		const float engineX = (c_EngineSize.x + margins.x) / 2.0f;
		Translate({ engineX, 0 });
	}

	const Vector2f SettingsButton::c_EngineSize = { 5 / 4.0f, 5 / 4.0f };

	const char* const SettingsButton::c_UnselectedTexturePath = 
	{
		"ui/button/UnselectedSettings.png"
	};
	const char* const SettingsButton::c_SelectedTexturePath = 
	{
		"ui/button/SelectedSettings.png"
	};
	const char* const SettingsButton::c_ClickedTexturePath =
	{
		"ui/button/ClickedSettings.png"
	};


	GameEndMenu::GameEndMenu(
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
	) :
		m_ProgramConfig(programConfig)
	{
		const auto createBackingFill =
			[&, this]()
			{
				constexpr Rect2f defaultEngineRect(Vector2f::Zero(), { 1, 1 });
				m_BackingFill = std::make_unique<RoundedRectangleSprite>(
					defaultEngineRect,
					backingColour,
					c_Roundness,
					coordTransformer,
					programConfig
				);
				m_BackingFill->SetParent(cameraEmpty);
			};

		const auto createBackingStroke =
			[&, this]()
			{
				constexpr Rect2f defaultEngineRect(Vector2f::Zero(), { 1, 1 });
				m_BackingStroke = std::make_unique<RoundedRectangleSprite>(
					defaultEngineRect,
					c_Black,
					c_Roundness,
					coordTransformer,
					programConfig,
					c_StrokeWidth
				);
				m_BackingStroke->SetParent(m_BackingFill.get());
			};

		const auto createMainMessage =
			[&, this]()
			{
				m_Message = std::make_unique<TextSprite>(
					Vector2f::Zero(),
					c_Black,
					c_MessageFontSize,
					messageText,
					coordTransformer,
					programConfig,
					textTextureLoader,
					font
				);
				m_Message->SetParent(m_BackingFill.get());
			};

		const auto createResetButton =
			[&, this]()
			{
				constexpr Vector2f initialPosition = Vector2f::Zero();
				m_ResetButton = std::make_unique<ResetButton>(
					coordTransformer,
					inputManager,
					programConfig,
					imageTextureLoader,
					resetCallback,
					initialPosition,
					m_BackingFill.get()
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

		const auto calculateSlideInStartPosition =
			[&, this]()
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
				m_SlideInStartPosition = { xPosition, 0 };
			};

		const auto updateResetButtonPosition =
			[this]()
			{
				const Vector2f halfBackingWidth = 
				{
					m_BackingFill->GetEngineSize() / 2
				};
				const Vector2f translation = 
				{
					halfBackingWidth.InvertY() + c_ResetButtonAdditionalOffset
				};
				m_ResetButton->Translate(translation);
			};

		createBackingFill();
		createBackingStroke();
		createMainMessage();
		createResetButton();

		updateEngineSize();
		calculateSlideInStartPosition();
		updateResetButtonPosition();
	}

	void GameEndMenu::Enable()
	{
		m_IsEnabled = true;
		m_SlideInTick = c_MaxSlideInTick;
	}

	void GameEndMenu::Update()
	{
		const auto updateSlidingIn =
			[this]()
			{
				float progress = 1 - (m_SlideInTick / c_MaxSlideInTick);
				progress = SineInterpolation(progress);

				const Vector2f enginePosition = Lerp(
					m_SlideInStartPosition,
					m_SlideInEndPosition,
					progress
				);
				m_BackingFill->SetLocalPosition(enginePosition);

				const float targetFrametime =
				{
					m_ProgramConfig->GetTargetFrametime()
				};
				m_SlideInTick -= targetFrametime;
			};

		if (m_IsEnabled)
		{
			m_Message->Update();
			m_ResetButton->Update();

			if (m_SlideInTick > 0)
				updateSlidingIn();
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
		}
	}

	const float GameEndMenu::c_Roundness = 1 / 4.0f;
	const float GameEndMenu::c_StrokeWidth = 2 / 44.0f;
	const float GameEndMenu::c_MessageFontSize = 96;
	const float GameEndMenu::c_MaxSlideInTick = 1 / 4.0f;

	const Vector2f GameEndMenu::c_Margins = { 3 / 10.0f, 1 / 10.0f };
	const Vector2f GameEndMenu::c_ResetButtonAdditionalOffset =
	{
		- 3 / 20.0f, -3 / 30.0f
	};


	WinMenu::WinMenu(
		const Callback& resetCallback,
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		TextTextureLoader* const textTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const Font* const font
	) :
		GameEndMenu(
			c_BackingColour,
			c_MessageText,
			resetCallback,
			coordTransformer,
			inputManager,
			programConfig,
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
		const CoordinateTransformer* const coordTransformer,
		const InputManager* const inputManager,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* const imageTextureLoader,
		TextTextureLoader* const textTextureLoader,
		const CameraEmpty* const cameraEmpty,
		const Font* const font
	) :
		GameEndMenu(
			c_BackingColour,
			c_MessageText,
			resetCallback,
			coordTransformer,
			inputManager,
			programConfig,
			imageTextureLoader,
			textTextureLoader,
			cameraEmpty,
			font
		)
	{

	}

	const Colour4i LoseMenu::c_BackingColour = Colour4i(0xFF, 0x80, 0x80);
	const char* const LoseMenu::c_MessageText = "Loser!";
}
