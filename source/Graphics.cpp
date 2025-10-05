#include "Graphics.h"

namespace BlastOff
{
	namespace
	{
		void ThrowPNGLoadingException(const string& resourcePath)
		{
			throw std::runtime_error(
				"Unable to load Texture from image "
				"at path \"" + resourcePath + "\"."
			);
		}

		Texture LoadPNG(const char* const resourcePath)
		{
			const static string start = string("resource/png/");
			const string resultingPath = start + resourcePath;
			const char* const cString = resultingPath.c_str();

			const Texture result = LoadTexture(cString);
			if (!result.id)
			{
				ThrowPNGLoadingException(resultingPath);
				return result;
			}
			else
				return result;
		}
	}

	RayWindow::RayWindow(
		const Vector2i size,
		const string& title
	) : m_Size(size)
	{
		InitWindow(size.x, size.y, title.c_str());
	}

	RayWindow::~RayWindow()
	{
		CloseWindow();
	}

	void RayWindow::Update()
	{
		const auto updateSize =
			[this]()
			{
				m_Size = Vector2i{ GetRenderWidth(), GetRenderHeight() };
			};

		UpdatePosition();
		updateSize();
	}

	const Vector2i* RayWindow::GetPosition() const
	{
		return &m_Position;
	}

	const Vector2i* RayWindow::GetSize() const
	{
		return &m_Size;
	}

	void RayWindow::UpdatePosition()
	{
		const RayVector2f unconverted = GetWindowPosition();
		const Vector2f converted =
		{
			Vector2f::FromRayVector2f(unconverted)
		};
		m_Position = converted.ToVector2i();
	}


	CoordinateTransformer::CoordinateTransformer(
		const Vector2i* const windowSize,
		const Vector2i* const windowPosition,
		const Vector2f* const cameraPosition
	) :
		m_WindowSize(windowSize),
		m_WindowPosition(windowPosition),
		m_CameraPosition(cameraPosition)
	{

	}

	float CoordinateTransformer::GetPixelsPerUnit() const
	{
		return m_PixelsPerUnit;
	}

	// TODO: try caching expensive method
	Rect2f CoordinateTransformer::
		ToScreenCoordinates(const Rect2f engineRect) const
	{
		const Vector2f windowCenter = (*m_WindowSize) / 2.0f;

		const Vector2f engineSize = engineRect.GetSize();
		const Vector2f enginePosition = engineRect.GetPosition();
		const Vector2f invertedPosition = enginePosition.InvertY();

		const Vector2f invPos = invertedPosition;
		const Vector2f camPos = *m_CameraPosition;
		const float ppu = m_PixelsPerUnit;

		const Vector2f resultingPosition = 
		{
			windowCenter + (((-engineSize / 2.0f) + invPos + camPos) * ppu)
		};
		const Vector2f resultingSize =
		{
			engineRect.GetSize() * m_PixelsPerUnit
		};

		const Rect2f result = Rect2f(resultingPosition, resultingSize);
		return result;
	}

	Vector2f CoordinateTransformer::ToEngineCoordinates
		(const Vector2i screenCoordinates) const
	{
		const Vector2f converted = screenCoordinates.ToVector2f();
		return ToEngineCoordinates(converted);
	}

	Vector2f CoordinateTransformer::ToEngineCoordinates
		(const Vector2f screenCoordinates) const
	{
		const Vector2f windowCenter = (*m_WindowSize) / 2.0f;
		const Vector2f windowPosition = m_WindowPosition->ToVector2f();

		const Vector2f camPos = *m_CameraPosition;
		const float ppu = m_PixelsPerUnit;
		const Vector2f winCenter = windowCenter;
		const Vector2f winPos = windowPosition;

		const Vector2f beforeCameraOffset =
		{
			((screenCoordinates - winCenter - winPos) / ppu)
		};
		const Vector2f resultingPosition = beforeCameraOffset.InvertY() + camPos;
		return resultingPosition;
	}

	float CoordinateTransformer::ScaleFontSize(const float fontSize) const
	{
		return fontSize * m_WindowSize->y / c_DevelopmentWindowHeight;
	}

	Vector2f CoordinateTransformer::GetViewportSize() const
	{
		return m_MostRecentWindowSize / m_PixelsPerUnit;
	}

	void CoordinateTransformer::Update()
	{
		const auto recomputePixelsPerUnit =
			[this]()
			{
				const Vector2f convertedSize = m_WindowSize->ToVector2f();
				const float squaredHypotenuse =
				{
					powf(convertedSize.x, 2) + powf(convertedSize.y, 2)
				};
				const float hypotenuse = sqrtf(squaredHypotenuse);
				m_PixelsPerUnit = hypotenuse * (1 / 8.0f);
			};

		if ((*m_WindowSize) != m_MostRecentWindowSize)
		{
			recomputePixelsPerUnit();
			m_MostRecentWindowSize = *m_WindowSize;
		}
	}

	const float CoordinateTransformer::c_DevelopmentWindowHeight = 960;


	Sprite::Sprite(
		const Rect2f engineRect,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig
	) : 
		m_EngineRect(engineRect),
		m_CoordTransformer(coordTransformer),
		m_ProgramConfig(programConfig)
	{

	}

	Vector2f Sprite::CalculateRealPosition() const
	{
		const auto calculateParentalOffset =
			[this]()
			{
				if (m_Parent)
					return m_Parent->CalculateRealPosition();
				else
					return Vector2f::Zero();
			};

		const auto calculateFromParentsRotation =
			[this]()
			{
				const Vector2f localPosition = m_EngineRect.GetPosition();
				const float parentRotation = m_Parent->CalculateRealRotation();

				return localPosition.Rotate(parentRotation);
			};

		const auto calculateRotatedLocalPosition =
			[&, this]()
			{
				const Vector2f localPosition = m_EngineRect.GetPosition();
				if (IsOrphan())
					return localPosition;
				else
				{
					const float parentRotation = m_Parent->CalculateRealRotation();
					if (parentRotation == 0)
						return localPosition;
					else
						return calculateFromParentsRotation();
				}
			};

		const Vector2f parentalOffset = calculateParentalOffset();
		const Vector2f rotatedLocalPosition = calculateRotatedLocalPosition();

		const Vector2f result = parentalOffset + rotatedLocalPosition;
		return result;
	}

	Vector2f Sprite::CalculateRealSize() const
	{
		const Vector2f engineSize = m_EngineRect.GetSize();
		const Vector2f result = engineSize * m_Scale;
		return result;
	}

	float Sprite::CalculateRealRotation() const
	{
		const auto calculateParentalRotation =
			[this]() -> float
			{
				if (m_Parent)
					return m_Parent->CalculateRealRotation();
				else
					return 0;
			};

		return m_LocalRotation + calculateParentalRotation();
	}

	Rect2f Sprite::CalculateRealRect() const
	{
		const Vector2f realPosition = CalculateRealPosition();
		const Vector2f realSize = CalculateRealSize();

		return Rect2f(realPosition, realSize);
	}

	Vector2f Sprite::GetLocalPosition() const
	{
		return m_EngineRect.GetPosition();
	}

	Vector2f Sprite::GetEngineSize() const
	{
		return m_EngineRect.GetSize();
	}

	Rect2f Sprite::GetEngineRect() const
	{
		return m_EngineRect;
	}

	Rect2f Sprite::GetCollisionRect() const
	{
		if (m_CollisionRect)
			return *m_CollisionRect;
		else
			return m_EngineRect;
	}

	Line2f Sprite::GetTopCollisionLine() const
	{
		const Rect2f collisionRect = GetCollisionRect();
		return collisionRect.GetTopLine();
	}

	Line2f Sprite::GetBottomLine() const
	{
		const Rect2f collisionRect = GetCollisionRect();
		return collisionRect.GetBottomLine();
	}

	Line2f Sprite::GetLeftCollisionLine() const
	{
		const Rect2f collisionRect = GetCollisionRect();
		return collisionRect.GetLeftLine();
	}

	Line2f Sprite::GetRightCollisionLine() const
	{
		const Rect2f collisionRect = GetCollisionRect();
		return collisionRect.GetRightLine();
	}

	void Sprite::ResetCollisionRect()
	{
		SetCollisionRect();
	}

	void Sprite::SetCollisionRect(const optional<Rect2f> collisionRect)
	{
		m_CollisionRect = collisionRect;
	}

	void Sprite::SetEngineRect(const Rect2f engineRect)
	{
		m_EngineRect = engineRect;
	}

	void Sprite::SetEngineSize(const Vector2f engineSize)
	{
		const Vector2f enginePosition = m_EngineRect.GetPosition();
		m_EngineRect = Rect2f(enginePosition, engineSize);
	}

	void Sprite::SetOpacity(const float opacity)
	{
		m_Opacity = opacity;
	}

	void Sprite::SetParent(const Sprite* parent)
	{
		m_Parent = parent;
	}

	void Sprite::SetScale(const float scale)
	{
		m_Scale = { scale, scale };
	}

	void Sprite::SetScale(const Vector2f scale)
	{
		m_Scale = scale;
	}

	void Sprite::SetLocalPosition(const Vector2f translation)
	{
		const Vector2f size = m_EngineRect.GetSize();
		m_EngineRect = Rect2f(translation, size);
	}

	void Sprite::SetLocalRotation(const float rotation)
	{
		m_LocalRotation = rotation;
	}

	bool Sprite::IsOrphan() const
	{
		return !m_Parent;
	}

	bool Sprite::CollideWithLine(const Line2f line) const
	{
		const Rect2f collisionRect = GetCollisionRect();
		return collisionRect.CollideWithLine(line);
	}

	void Sprite::Move(const Vector2f translation)
	{
		m_EngineRect = m_EngineRect.Translate(translation);
	}

	void Sprite::Rotate(const float rotation)
	{
		m_LocalRotation += rotation;
	}

	void Sprite::Update()
	{
		// nothing to do here.
	}

	
	Empty::Empty(
		const Vector2f enginePosition,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig
	) :
		Sprite(
			SetEnginePosition(enginePosition), 
			coordTransformer, 
			programConfig
		)
	{

	}

	void Empty::Draw() const
	{
		// nothing to do here.
	}

	Rect2f Empty::SetEnginePosition(const Vector2f enginePosition)
	{
		return Rect2f(enginePosition, Vector2f::Zero());
	}


	CameraEmpty::CameraEmpty(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		const Vector2f* const cameraPosition
	) :
		Empty(
			*cameraPosition,
			coordTransformer, 
			programConfig
		),
		m_CameraPosition(cameraPosition)
	{

	}

	void CameraEmpty::Update()
	{
		const Rect2f newRect = SetEnginePosition(*m_CameraPosition);
		SetEngineRect(newRect);
	}


	GradientSprite::GradientSprite(
		const Rect2f engineRect,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		const Colour4i firstColour,
		const Colour4i secondColour,
		const Direction direction
	) :
		Sprite(engineRect, coordTransformer, programConfig),
		m_FirstColour(firstColour),
		m_SecondColour(secondColour),
		m_Direction(direction)
	{
		
	}

	void GradientSprite::Draw() const
	{
		struct CornerColours
		{
			Colour4i topLeft;
			Colour4i topRight;
			Colour4i bottomLeft;
			Colour4i bottomRight;
		};

		const auto calculateCornerColours =
			[&, this]() -> CornerColours
			{
				switch (m_Direction)
				{
				case Direction::Up:
					return CornerColours
					{
						.topLeft = m_SecondColour,
						.topRight = m_SecondColour,
						.bottomLeft = m_FirstColour,
						.bottomRight = m_FirstColour
					};

				case Direction::Down:
					return CornerColours
					{
						.topLeft = m_FirstColour,
						.topRight = m_FirstColour,
						.bottomLeft = m_SecondColour,
						.bottomRight = m_SecondColour
					};

				case Direction::Left:
					return CornerColours
					{
						.topLeft = m_SecondColour,
						.topRight = m_FirstColour,
						.bottomLeft = m_SecondColour,
						.bottomRight = m_FirstColour
					};

				case Direction::Right:
					return CornerColours
					{
						.topLeft = m_FirstColour,
						.topRight = m_SecondColour,
						.bottomLeft = m_FirstColour,
						.bottomRight = m_SecondColour
					};

				default:
					return CornerColours
					{
						.topLeft = m_ProgramConfig->GetInvalidColour1(),
						.topRight = m_ProgramConfig->GetInvalidColour1(),
						.bottomLeft = m_ProgramConfig->GetInvalidColour1(),
						.bottomRight = m_ProgramConfig->GetInvalidColour1()
					};
				}
			};

		const Rect2f realRect = CalculateRealRect();
		const Rect2f drawRect = 
		{
			m_CoordTransformer->ToScreenCoordinates(realRect)
		};
		const RayRect2f convertedDrawRect = drawRect.ToRayRect2f();

		const CornerColours cornerColours = calculateCornerColours();
		DrawRectangleGradientEx(
			convertedDrawRect,
			cornerColours.topLeft.ToRayColour(),
			cornerColours.bottomLeft.ToRayColour(),
			cornerColours.bottomRight.ToRayColour(),
			cornerColours.topRight.ToRayColour()
		);
	}


	ImageTextureLoader::~ImageTextureLoader()
	{
		for (const auto& [name, texture] : m_CachedValues)
		{
			(void)name;
			UnloadTexture(texture);
		}
	}

	const Texture* ImageTextureLoader::LazyLoadTexture
		(const char* const resourcePath)
	{
		const auto getCachedValue =
			[&, this]() -> const Texture*
			{
				try
				{
					return &m_CachedValues.at(resourcePath);
				}
				catch (const std::out_of_range& exception)
				{
					(void)exception;
					return nullptr;
				}
			};

		const auto result = getCachedValue();
		if (!result)
			return LoadAndInsert(resourcePath);
		else
			return result;
	}

	const Texture* ImageTextureLoader::LoadAndInsert
		(const char* const resourcePath)
	{
		const Texture result = LoadPNG(resourcePath);
		m_CachedValues.insert({ resourcePath, result });

		return &m_CachedValues.at(resourcePath);
	}


	bool TextTextureParameters::operator==
		(const TextTextureParameters& other) const
	{
		return
			(fontSize == other.fontSize) &&
			(spacing == other.spacing) &&
			(colour == other.colour) &&
			(message == other.message);
	}

	size_t TextTextureParameters::Hasher::operator()
		(const TextTextureParameters& key) const
	{
		const int* colourInt = (int*)(&key.colour);
		using std::hash;

		constexpr size_t start = 17;
		constexpr size_t multiplier = 31;

		size_t result = start;
		result = (result * multiplier) + hash<float>()(key.fontSize);
		result = (result * multiplier) + hash<float>()(key.spacing);
		result = (result * multiplier) + hash<int>()(*colourInt);
		result = (result * multiplier) + hash<string>()(key.message);
		return result;
	}


	TextTextureLoader::TextTextureLoader(const Font* const font) :
		m_Font(font)
	{

	}

	TextTextureLoader::~TextTextureLoader()
	{
		for (const auto& [key, texture] : m_CachedValues)
		{
			(void)key;
			UnloadTexture(texture);
		}
	}

	const Texture* TextTextureLoader::LazyLoadTexture
		(const Parameters& parameters)
	{
		const auto getCachedValue =
			[&, this]() -> const Texture*
			{
				try
				{
					return &m_CachedValues.at(parameters);
				}
				catch (const std::out_of_range& exception)
				{
					(void)exception;
					return nullptr;
				}
			};

		const auto result = getCachedValue();
		if (!result)
			return LoadAndInsert(parameters);
		else
			return result;
	}

	const Texture* TextTextureLoader::LoadAndInsert
		(const Parameters& parameters)
	{
		const Image image = ImageTextEx(
			*m_Font,
			parameters.message.c_str(),
			parameters.fontSize,
			parameters.spacing,
			parameters.colour.ToRayColour()
		);
		const Texture result = LoadTextureFromImage(image);
		
		m_CachedValues.insert({ parameters, result });
		return &m_CachedValues.at(parameters);
	}


	ImageSprite::ImageSprite(
		const Rect2f engineRect,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		const Texture* const texture
	) :
		Sprite(
			engineRect, 
			coordTransformer, 
			programConfig
		)
	{
		SetTexture(texture);
	}

	ImageSprite::ImageSprite(
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		const Texture* const texture
	) :
		ImageSprite(
			Rect2f::UnitRect(),
			coordTransformer,
			programConfig,
			texture
		)
	{

	}

	unique_ptr<ImageSprite> ImageSprite::LoadFromPath(
		const char* const resourcePath,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* imageTextureLoader
	)
	{
		constexpr Rect2f engineRect = Rect2f::UnitRect();
		return LoadFromPath(
			engineRect,
			resourcePath,
			coordTransformer,
			programConfig,
			imageTextureLoader
		);
	}

	unique_ptr<ImageSprite> ImageSprite::LoadFromPath(
		const Rect2f engineRect,
		const char* const resourcePath,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		ImageTextureLoader* imageTextureLoader
	)
	{
		const Texture* texture = imageTextureLoader->LazyLoadTexture(resourcePath);
		return std::make_unique<ImageSprite>(
			engineRect,
			coordTransformer,
			programConfig,
			texture
		);
	}

	void ImageSprite::SetTexture(const Texture* const texture)
	{
		m_Texture = texture;
		if (m_Texture)
			SetTextureFilter(*m_Texture, c_TextureFiltering);
	}

	void ImageSprite::Draw() const
	{
		const Vector2i textureSize = { m_Texture->width, m_Texture->height };
		const Rect2f sourceRect = m_Crop.CalculateSourceRect(textureSize);

		const Rect2f realRect = CalculateRealRect();
		Rect2f drawRect =
		{
			m_CoordTransformer->ToScreenCoordinates(realRect)
		};

		const Colour4i drawTint = c_DefaultDrawTint.WithOpacity(m_Opacity);

		// raylib is weird, so i need to do this for some reason?
		const Vector2f drawOrigin = drawRect.GetSize() / 2.0f;
		drawRect = drawRect.Translate(drawOrigin);

		DrawTexturePro(
			*m_Texture,
			sourceRect.ToRayRect2f(),
			drawRect.ToRayRect2f(),
			drawOrigin.ToRayVector2f(),
			CalculateRealRotation(),
			drawTint.ToRayColour()
		);
	}
	
	Vector2f ImageSprite::CalculateRealPosition() const
	{
		const Vector2f baseResult = Sprite::CalculateRealPosition();
		const Vector2f cropOffset = CalculateCropPositionOffset();

		return baseResult + cropOffset;
	}
	
	Vector2f ImageSprite::CalculateRealSize() const
	{
		const Vector2i textureSize = { m_Texture->width, m_Texture->height };
		const Rect2f sourceRect = m_Crop.CalculateSourceRect(textureSize);

		const Vector2f baseResult = Sprite::CalculateRealSize();
		const Vector2f cropMultiplier =
		{
			CalculateCropSizeMultiplier(sourceRect.GetSize())
		};

		return baseResult * cropMultiplier;
	}

	// TODO: try caching expensive method
	Vector2f ImageSprite::CalculateCropPositionOffset() const
	{
		const Rect2f rect = m_EngineRect;
		const Vector2f originalPosition = rect.GetPosition();

		constexpr float half = 0.5f;

		const float leftBalance = m_Crop.left * rect.w * half;
		const float rightBalance = -(m_Crop.right * rect.w * half);
		const float topBalance = -(m_Crop.top * rect.h * half);
		const float bottomBalance = m_Crop.bottom * rect.h * half;

		const Vector2f translation =
		{
			leftBalance + rightBalance,
			topBalance + bottomBalance
		};
		return translation;
	}

	Vector2f ImageSprite::CalculateCropSizeMultiplier
		(const Vector2f croppedSize) const
	{
		const Vector2f textureSize = 
		{ 
			(float)m_Texture->width, 
			(float)m_Texture->height 
		};
		const Vector2f cropRatio = croppedSize / textureSize;
		return cropRatio;
	}

	Vector2i ImageSprite::GetImageSize() const
	{
		return Vector2i{ m_Texture->width, m_Texture->height };
	}

	void ImageSprite::SetCrop(const Crop2f crop)
	{
		m_Crop = crop;
	}

	const int ImageSprite::c_TextureFiltering = TEXTURE_FILTER_POINT;


	TextSprite::TextSprite(
		const Vector2f enginePosition,
		const Colour4i colour,
		const float fontSize,
		const char* const message,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		TextTextureLoader* const textureLoader,
		const Font* const font
	) :
		ImageSprite(
			Rect2f(enginePosition, Vector2f::Zero()),
			coordTransformer,
			programConfig,
			nullptr
		),
		m_Colour(colour),
		m_FontSize(fontSize),
		m_Message(message),
		m_TextureLoader(textureLoader),
		m_Font(font)
	{

	}

	Colour4i TextSprite::GetColour() const
	{
		return m_Colour;
	}

	void TextSprite::SetEngineRect(const Rect2f engineRect)
	{
		Sprite::SetEngineRect(engineRect);

		m_ShouldRecacheTexture = true;
	}

	void TextSprite::SetScale(const float scale)
	{
		Sprite::SetScale(scale);

		m_ShouldRecacheTexture = true;
	}

	void TextSprite::SetScale(const Vector2f scale)
	{
		Sprite::SetScale(scale);

		m_ShouldRecacheTexture = true;
	}

	void TextSprite::Update()
	{
		const auto recalculateEngineSize =
			[this]()
			{
				const Parameters parameters = CalculateParameters();
				const RayVector2f measurement = MeasureTextEx(
					*m_Font,
					parameters.message.c_str(),
					parameters.fontSize,
					parameters.spacing
				);

				const float ppu = m_CoordTransformer->GetPixelsPerUnit();
				Vector2f convertedMeasurement =
				{
					Vector2f::FromRayVector2f(measurement)
				};
				convertedMeasurement /= ppu;

				const Vector2f enginePosition = m_EngineRect.GetPosition();
				m_EngineRect = Rect2f(enginePosition, convertedMeasurement);
			};

		const auto lazyLoadTexture =
			[this]()
			{
				const Parameters parameters = CalculateParameters();
				const Texture* const newTexture = 
				{
					m_TextureLoader->LazyLoadTexture(parameters)
				};
				SetTexture(newTexture);
			};

		if (m_ShouldRecacheTexture)
		{
			recalculateEngineSize();
			lazyLoadTexture();
			m_ShouldRecacheTexture = false;
		}
	}

	const float TextSprite::c_SpacingPer24 = 2;

	TextTextureParameters TextSprite::CalculateParameters() const
	{
		const float scaledFontSize =
		{
			m_CoordTransformer->ScaleFontSize(m_FontSize)
		};
		const float unscaledSpacing = CalculateSpacing();
		const float scaledSpacing =
		{
			m_CoordTransformer->ScaleFontSize(unscaledSpacing)
		};
		const TextTextureParameters parameters =
		{
			.fontSize = scaledFontSize,
			.spacing = scaledSpacing,
			.colour = m_Colour,
			.message = m_Message
		};
		return parameters;
	}

	float TextSprite::CalculateSpacing() const
	{
		return m_FontSize * c_SpacingPer24 / 24;
	}


	RoundedRectangleSprite::RoundedRectangleSprite(
		const Rect2f engineRect,
		const Colour4i colour,
		const float roundness,
		const CoordinateTransformer* const coordTransformer,
		const ProgramConfiguration* const programConfig,
		const optional<float> strokeWidth
	) :
		Sprite(
			engineRect,
			coordTransformer,
			programConfig
		),
		m_Colour(colour),
		m_Roundness(roundness)
	{
		if (strokeWidth)
		{
			m_Type = Type::StrokeOnly;
			m_StrokeWidth = *strokeWidth;
		}
		else
			m_Type = Type::FillOnly;
	}

	void RoundedRectangleSprite::Draw() const
	{
		const Rect2f realRect = CalculateRealRect();
		const Rect2f drawRect = 
		{
			m_CoordTransformer->ToScreenCoordinates(realRect)
		};

		switch (m_Type)
		{
		case Type::FillOnly:
			DrawRectangleRounded(
				drawRect.ToRayRect2f(),
				m_Roundness,
				c_Resolution,
				m_Colour.ToRayColour()
			);

		case Type::StrokeOnly:
			DrawRectangleRoundedLinesEx(
				drawRect.ToRayRect2f(),
				m_Roundness,
				c_Resolution,
				CalculateScreenStrokeWidth(),
				m_Colour.ToRayColour()
			);
		}
	}

	const int RoundedRectangleSprite::c_Resolution = 10;

	float RoundedRectangleSprite::CalculateScreenStrokeWidth() const
	{
		const float ppu = m_CoordTransformer->GetPixelsPerUnit();
		return m_StrokeWidth * ppu;
	}
}
