#pragma once

#include "Utils.h"
#include "ProgramConstants.h"

#include "raylib.h"

namespace BlastOff
{
	// Sept. 27th, 2025:
	// 
	// NONE of this code is final. probably contains lots of missing features.
	// (rotation, scale, etc.)
	// this is because the code has only been used in BlastOff at this point.
	// expect this to be finished some time before the next decade :)
	//
	// - Andrew C.

	struct ShapeColours
	{
		Colour4i stroke;
		Colour4i fill;
	};

	struct RayWindow
	{
		RayWindow(const Vector2i size, const string& title);
		~RayWindow();

		void Update();

		const Vector2i* GetPosition() const;
		const Vector2i* GetSize() const;

		void SetPosition(const Vector2i position);
		void SetSize(const Vector2i size);

	private:
		Vector2i m_Position = Vector2i::Zero();
		Vector2i m_Size = Vector2i::Zero();

		void UpdatePosition();
	};

	struct CoordinateTransformer
	{
		CoordinateTransformer(
			const Vector2i* const windowSize,
			const Vector2i* const windowPosition,
			const Vector2f* const cameraPosition
		);

		float GetFontPixelsPerUnit() const;
		float GetPixelsPerUnit() const;

		Rect2f ToScreenCoordinates(const Rect2f engineRect) const;

		Vector2f ToEngineCoordinates(const Vector2f screenCoordinates) const;
		Vector2f ToEngineCoordinates(const Vector2i screenCoordinates) const;

		float ScaleTextureFontSize(const float fontSize) const;

		Vector2f GetViewportSize() const;

		void Update();

	private:
		// if dynamic window sizing is true, 
		// all text textures should be scaled as if they're on a large screen
		// to avoid lazy-loaded textures being low quality
		static constexpr bool c_DynamicWindowSizing = true;

		static const float c_DevelopmentWindowHeight;

		const Vector2i* m_WindowSize = nullptr;
		const Vector2i* m_WindowPosition = nullptr;
		const Vector2f* m_CameraPosition = nullptr;

		Vector2i m_MostRecentWindowSize = Vector2i::Zero();

		// the PixelsPerUnit is calculated 
		// once every frame to save on computation.
		//
		// it requires sqrtf() calls, which are expensive on some platforms
		float m_PixelsPerUnit = 1;
	};

	struct Sprite
	{
		Sprite(
			const Rect2f engineRect,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig
		);
		virtual ~Sprite()
		{

		}

		float GetEdgePosition(const Direction side) const;

		virtual Vector2f CalculateRealPosition() const;
		virtual Vector2f CalculateRealSize() const;
		Rect2f CalculateRealRect() const;
		float CalculateRealRotation() const;

		Vector2f GetLocalPosition() const;
		Vector2f GetEngineSize() const;
		Rect2f GetEngineRect() const;
		Rect2f GetCollisionRect() const;

		Line2f GetTopCollisionLine() const;
		Line2f GetBottomLine() const;
		Line2f GetLeftCollisionLine() const;
		Line2f GetRightCollisionLine() const;

		void ResetCollisionRect();
		void SetCollisionRect
			(const optional<Rect2f> collisionRect = std::nullopt);

		virtual void SetEngineRect(const Rect2f engineRect);
		virtual void SetEngineSize(const Vector2f engineSize);

		virtual void SetScale(const float scale);
		virtual void SetScale(const Vector2f scale);

		void SetOpacity(const float opacity);
		void SetParent(const Sprite* parent);

		void SetLocalPosition(const Vector2f position);
		void SetLocalRotation(const float rotation);

		bool IsOrphan() const;

		bool CollideWithLine(const Line2f line) const;

		void Move(const Vector2f translation);
		void Rotate(const float rotation);

		virtual void Update();
		virtual void Draw() const = 0;

	protected:
		float m_LocalRotation = 0;
		float m_Opacity = 1;

		Vector2f m_Scale = Vector2f(1, 1);
		Rect2f m_EngineRect = Rect2f::UnitRect();
		optional<Rect2f> m_CollisionRect = std::nullopt;

		const Sprite* m_Parent = nullptr;

		const CoordinateTransformer* m_CoordTransformer = nullptr;
		const ProgramConstants* m_ProgramConfig = nullptr;
	};

	struct Empty : public Sprite
	{
		Empty(
			const Vector2f enginePosition,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig
		);

		void Draw() const override;

	protected:
		static Rect2f SetEnginePosition(const Vector2f enginePosition);
	};

	struct CameraEmpty : public Empty
	{
		CameraEmpty(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig,
			const Vector2f* const cameraPosition
		);

		void Update();

	protected:
		const Vector2f* m_CameraPosition = nullptr;
	};

	struct GradientSprite : public Sprite
	{
		GradientSprite(
			const Rect2f engineRect,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig,
			const Colour4i firstColour, 
			const Colour4i secondColour, 
			const Direction direction
		);

		void Draw() const override;

	protected:
		Colour4i m_FirstColour = c_White;
		Colour4i m_SecondColour = c_Black;
		Direction m_Direction = Direction::Down;
	};

	struct ImageTextureLoader
	{
		static const int c_DefaultTextureFiltering;

		~ImageTextureLoader();
		const Texture* LazyLoadTexture(const char* const resourcePath);

	private:
		unordered_map<string, Texture> m_CachedValues = {};
		const Texture* LoadAndInsert(const char* const resourcePath);
	};

	struct TextTextureParameters
	{
		float fontSize;
		float spacing;
		Colour4i colour;
		string message;

		bool operator==(const TextTextureParameters& other) const;

		struct Hasher
		{
			size_t operator()(const TextTextureParameters& key) const;
		};
	};

	struct TextTextureLoader
	{
		using Parameters = TextTextureParameters;

		TextTextureLoader(const Font* const font);
		~TextTextureLoader();

		const Texture* LazyLoadTexture(const Parameters& parameters);

	private:
		using CacheMap = unordered_map<Parameters, Texture, Parameters::Hasher>;

		CacheMap m_CachedValues = {};
		
		const Font* m_Font;

		const Texture* LoadAndInsert(const Parameters& parameters);
	};

	struct ImageSprite : public Sprite
	{
		ImageSprite(
			const Rect2f engineRect,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig,
			const Texture* const texture
		);
		ImageSprite(
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig,
			const Texture* const texture
		);

		static unique_ptr<ImageSprite> LoadFromPath(
			const char* const resourcePath,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig,
			ImageTextureLoader* imageTextureLoader
		);
		static unique_ptr<ImageSprite> LoadFromPath(
			const Rect2f engineRect,
			const char* const resourcePath,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig,
			ImageTextureLoader* imageTextureLoader
		);

		void SetTexture(const Texture* const texture);

		void Draw() const override;

		Vector2f CalculateRealPosition() const override;
		Vector2f CalculateRealSize() const override;

		Vector2f CalculateCropPositionOffset() const;
		Vector2f CalculateCropSizeMultiplier(const Vector2f croppedSize) const;

		Vector2i GetImageSize() const;

		void SetCrop(const Crop2f crop);

	protected:
		static constexpr Colour4i c_DefaultDrawTint = c_White;

		const Texture* m_Texture = nullptr;
		Crop2f m_Crop = { 0 };
	};

	struct TextSprite : public ImageSprite
	{
		TextSprite(
			const Vector2f enginePosition,
			const Colour4i colour,
			const float fontSize,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig,
			TextTextureLoader* const textureLoader,
			const Font* const font,
			const string& message = ""
		);

		Colour4i GetColour() const;

		void SetMessage(const string& message);

		void SetEngineRect(const Rect2f engineRect) override;

		void SetScale(const float scale) override;
		void SetScale(const Vector2f scale) override;

		void Update() override;

	protected:
		using Parameters = TextTextureParameters;

		static const float c_SpacingPer24;

		bool m_ShouldRecacheTexture = true;

		Colour4i m_Colour = c_White;
		float m_FontSize = 0;
		string m_Message = "";
		const Font* m_Font = nullptr;
		TextTextureLoader* m_TextureLoader = nullptr;

		Parameters CalculateParameters() const;
		float CalculateSpacing() const;
	};

	enum class ShapeSpriteType
	{
		Invalid = 0,
		StrokeOnly,
		FillOnly
	};

	struct RoundedRectangleSprite : public Sprite
	{
		RoundedRectangleSprite(
			const Rect2f engineRect,
			const Colour4i colour,
			const float roundness,
			const CoordinateTransformer* const coordTransformer,
			const ProgramConstants* const programConfig,
			const optional<float> strokeWidth = std::nullopt
		);

		void SetColour(const Colour4i colour);

		void Draw() const override;

	protected:
		using Type = ShapeSpriteType;

		static const int c_Resolution;

		Type m_Type = Type::Invalid;
		Colour4i m_Colour = c_White;
		float m_Roundness = 0;
		float m_StrokeWidth = 0;

		float CalculateScreenStrokeWidth() const;
	};
}
