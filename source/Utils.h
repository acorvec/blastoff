#pragma once

#include <optional>
#include <string>
#include <memory>
#include <vector>
#include <map>
#include <array>
#include <sstream>
#include <functional>
#include <cstdint>
#include <expected>
#include <chrono>

#define _USE_MATH_DEFINES
#include <math.h>

#include "OperatingSystem.h"
#include "Debug.h"
#include "JSONDefs.h"

#if !USE_GLAZE
#include <rapidjson/document.h>
#include <rapidjson/stringbuffer.h>
#include <rapidjson/prettywriter.h>
#endif

#if COMPILE_CONFIG_DEBUG
// includes used for debugging only
#include <csignal>
#include <print>
#include <cstdio>
#endif

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

	using std::unique_ptr;
	using std::optional, std::expected;
	using std::string, std::wstring, std::stringstream;
	using std::string_view, std::wstring_view;
	using std::vector, std::array;
	using std::function;
	using std::unordered_map, std::map;

	using namespace std::chrono;

	using byte = uint8_t;

	using Callback = function<void()>;

	using RayColour = Color;
	using RayVector2f = Vector2;
	using RayRect2f = Rectangle;

#if !USE_GLAZE
	using namespace rapidjson;
#endif

	static inline constexpr int c_DeactivatedTick = -1;
	static inline constexpr int c_DeactivatedTracker = -1;

	static inline constexpr float c_Pi = (float)M_PI;
	
	static inline constexpr float c_ToDegreesMultiplier = 180 / c_Pi;
	static inline constexpr float c_ToRadiansMultiplier = c_Pi / 180;

	enum class Direction
	{
		Up,
		Down,
		Left,
		Right,
		None
	};

	constexpr array<Direction, 4> GetDirectionsWithoutNone()
	{
		using enum Direction;
		return { Up, Down, Left, Right };
	};

	constexpr string DirectionToString(const Direction value)
	{
		switch (value)
		{
		case Direction::Up:
			return "Direction::Up";

		case Direction::Down:
			return "Direction::Down";

		case Direction::Left:
			return "Direction::Left";

		case Direction::Right:
			return "Direction::Right";

		case Direction::None:
			return "Direction::None";

		default:
			return "Direction::INVALID";
		}
	}

	constexpr optional<Direction> GetOppositeDirection(const Direction value)
	{
		switch (value)
		{
		case Direction::Up:
			return Direction::Down;

		case Direction::Down:
			return Direction::Up;

		case Direction::Left:
			return Direction::Right;

		case Direction::Right:
			return Direction::Left;

		default:
			return std::nullopt;
		}
	}

	constexpr float ToRadians(const float degrees)
	{
		return degrees * c_ToRadiansMultiplier;
	}

	constexpr float ToDegrees(const float radians)
	{
		return radians * c_ToDegreesMultiplier;
	}

	constexpr float Lerp(const float a, const float b, const float t)
	{
		return ((1 - t) * a) + (t * b);
	}

	constexpr float ReverseLerp(
		const float a, 
		const float b, 
		const float result
	)
	{
		return (result - a) / (b - a);
	}

	string ByteToHexString(const byte value);
	string BoolToString(const bool value);

	vector<string> SplitString(const char* const value, const char delim);

	bool StringContains(const char* const string, const char value);

	float GetRandomFloat();
	float RoundToFraction(const float num, const float fraction);
	float FloorToFraction(const float num, const float fraction);
	
	float SineInterpolation(const float number);
	float NthSineInterpolation(
		const float number,
		const float applications
	);
	float DoubleSineInterpolation(const float number);

	struct Edge2f
	{
		Direction side;
		float position;

		optional<float> GetSign() const;
	};

	struct Edge2i
	{
		Direction side;
		int position;
	};

	struct Vector2i;

	struct Vector2f
	{
		float x = 0, y = 0;

		Vector2f() = delete;

		constexpr Vector2f(const float x, const float y)
		{
			this->x = x;
			this->y = y;
		}

		constexpr Vector2f(const RayVector2f rayVector) :
			x(rayVector.x), y(rayVector.y)
		{

		}

		constexpr Vector2f operator+() const
		{
			return *this;
		}

		constexpr Vector2f operator-() const
		{
			return { -x, -y };
		}

		constexpr Vector2f operator+(const Vector2f other) const
		{
			return Vector2f
			{
				x + other.x, y + other.y
			};
		}

		constexpr Vector2f operator-(const Vector2f other) const
		{
			return Vector2f
			{
				x - other.x, y - other.y
			};
		}

		constexpr Vector2f operator*(const float scalar) const
		{
			return Vector2f
			{
				x * scalar, y * scalar
			};
		}

		constexpr Vector2f operator*(const Vector2f factor) const
		{
			return { x * factor.x, y * factor.y };
		}

		constexpr Vector2f operator/(const float scalar) const
		{
			return Vector2f
			{
				x / scalar, y / scalar
			};
		}

		constexpr Vector2f operator/(const Vector2f divisor) const
		{
			return { x / divisor.x, y / divisor.y };
		}

		constexpr Vector2f operator+=(const Vector2f other)
		{
			x = x + other.x;
			y = y + other.y;
			return *this;
		}

		constexpr Vector2f operator-=(const Vector2f other)
		{
			x = x - other.x;
			y = y - other.y;
			return *this;
		}

		constexpr Vector2f operator*=(const float scalar)
		{
			x = x * scalar;
			y = y * scalar;
			return *this;
		}

		constexpr Vector2f operator/=(const float scalar)
		{
			x = x / scalar;
			y = y / scalar;
			return *this;
		}

		constexpr bool operator == (const Vector2f other) const
		{
			return x == other.x && y == other.y;
		}

		constexpr bool operator != (const Vector2f other) const
		{
			return !(*this == other);
		}

		constexpr Vector2f Negate() const
		{
			return Vector2f{ 0, 0 } - (*this);
		}

		constexpr float Area() const
		{
			return x * y;
		}

		constexpr float Dot(const Vector2f other) const
		{
			return x * other.x + y * other.y;
		}

		constexpr Vector2f InvertX() const
		{
			return { -x, y };
		}

		constexpr Vector2f InvertY() const
		{
			return { x, -y };
		}

		constexpr bool CollideWithEdge(const Edge2f edge) const
		{
			switch (edge.side)
			{
			case Direction::Left:
				return x < edge.position;

			case Direction::Right:
				return x > edge.position;

			case Direction::Up:
				return y > edge.position;

			case Direction::Down:
				return y < edge.position;

			default:
				return false;
			}
		}

		constexpr static Vector2f FromRayVector2f(const RayVector2f value)
		{
			return { value.x, value.y };
		}

		constexpr RayVector2f ToRayVector2f() const
		{
			return { x, y };
		}

		optional<float> Angle(const bool useRadians = false) const;

		Vector2f Rotate(
			const float angle, 
			const bool useRadians = false
		) const;
		Vector2f Normalize() const;

		float Distance(const Vector2f other) const;
		float Magnitude() const;

		Vector2i CeilToVector2i() const;
		Vector2i ToVector2i() const;
		Vector2f RoundToFraction(const float fraction) const;

		operator string() const;

		static constexpr Vector2f Zero()
		{
			return { 0, 0 };
		}

		static constexpr Vector2f Up()
		{
			return { 0, 1 };
		}

		static constexpr Vector2f Down()
		{
			return { 0, -1 };
		}

		static constexpr Vector2f Left()
		{
			return { -1, 0 };
		}

		static constexpr Vector2f Right()
		{
			return { 1, 0 };
		}
	};

	struct Vector2i
	{
		int x = 0, y = 0;

		Vector2i() = delete;

		constexpr Vector2i(const int x, const int y)
		{
			this->x = x;
			this->y = y;
		}

		constexpr Vector2i(const CursorPosition cursorPosition)
		{
			x = cursorPosition.x;
			y = cursorPosition.y;
		}

		constexpr Vector2i operator+() const
		{
			return *this;
		}

		constexpr Vector2i operator-() const
		{
			return Vector2i{ -x, -y };
		}

		constexpr Vector2i operator+(const Vector2i other) const
		{
			return Vector2i{ x + other.x, y + other.y };
		}

		constexpr Vector2i operator-(const Vector2i other) const
		{
			return Vector2i{ x - other.x, y - other.y };
		}

		constexpr Vector2f operator+(const Vector2f other) const
		{
			return { x + other.x, y + other.y };
		}

		constexpr Vector2f operator-(const Vector2f other) const
		{
			return { x - other.x, y - other.y };
		}

		constexpr Vector2f operator*(const float scalar) const
		{
			return { x * scalar, y * scalar};
		}

		constexpr Vector2f operator/(const float scalar) const
		{
			return { x / scalar, y / scalar };
		}

		constexpr bool operator == (const Vector2i other) const
		{
			return x == other.x && y == other.y;
		}

		constexpr bool operator != (const Vector2i other) const
		{
			return !(*this == other);
		}

		constexpr Vector2i Negate() const
		{
			return -(*this);
		}

		constexpr int Area() const
		{
			return x * y;
		}

		constexpr float Dot(const Vector2f other) const
		{
			return x * other.x + y * other.y;
		}

		constexpr Vector2i InvertY() const
		{
			return Vector2i{ x, -y };
		}

		constexpr bool CollideWithEdge(const Edge2i edge) const
		{
			switch (edge.side)
			{
			case Direction::Up:
				return y > edge.position;

			case Direction::Down:
				return y < edge.position;

			case Direction::Left:
				return x < edge.position;

			case Direction::Right:
				return x > edge.position;

			default:
				return false;
			}
		}

		constexpr Vector2f ToVector2f() const
		{
			return { (float)x, (float)y };
		}

		constexpr RayVector2f ToRayVector2f() const
		{
			return { (float)x, (float)y };
		}

		static Vector2i FromRayVector2f(const RayVector2f value);
#if !USE_GLAZE
		static Vector2i FromJSONValue(const Value& value);
		void WriteToJSONWriter(Writer<StringBuffer>& writer) const;
#endif

		Vector2f Normalize() const;
		float Magnitude() const;

		operator string() const;

		static constexpr Vector2i Zero()
		{
			return Vector2i{ 0, 0 };
		}

		static constexpr Vector2i Up()
		{
			return Vector2i{ 0, 1 };
		}

		static constexpr Vector2i Down()
		{
			return Vector2i{ 0, -1 };
		}

		static constexpr Vector2i Left()
		{
			return Vector2i{ -1, 0 };
		}

		static constexpr Vector2i Right()
		{
			return Vector2i{ 1, 0 };
		}
	};

	struct Line2f
	{
		Vector2f start = Vector2f::Zero();
		Vector2f end = Vector2f::Up();

		Line2f() = delete;
			 
		constexpr Line2f(const Vector2f start, const Vector2f end)
		{
			this->start = start;
			this->end = end;
		}

		constexpr float AverageX() const
		{
			return (start.x + end.x) / 2.0f;
		}

		constexpr float AverageY() const
		{
			return (start.y + end.y) / 2.0f;
		}

		constexpr Vector2f CenterPosition() const
		{
			return { AverageX(), AverageY() };
		}

		constexpr bool Collide(const Line2f other) const
		{
			// http://jeffreythompson.org/collision-detection/line-line.php
			const Vector2f s1 = start, s2 = end;
			const Vector2f o1 = other.start, o2 = other.end;

			const float x1 = s1.x, x2 = s2.x;
			const float x3 = o1.x, x4 = o2.x;
			const float y1 = s1.y, y2 = s2.y;
			const float y3 = o1.y, y4 = o2.y;

			const float divisor1 = 
			{
				(y4 - y3)* (x2 - x1) - (x4 - x3) * (y2 - y1)
			};
			if (divisor1 == 0)
				return false;

			const float divisor2 =
			{
				(y4 - y3) * (x2 - x1) - (x4 - x3) * (y2 - y1)
			};
			if (divisor2 == 0)
				return false;

			const float difference1 =
			{
				((x4 - x3) * (y1 - y3) - (y4 - y3) * (x1 - x3)) / divisor1
			};
			const float difference2 =
			{
				((x2 - x1) * (y1 - y3) - (y2 - y1) * (x1 - x3)) / divisor2
			};

			const bool isInRange1 = (difference1 >= 0) && (difference1 <= 1);
			const bool isInRange2 = (difference2 >= 0) && (difference2 <= 1);

			return isInRange1 && isInRange2;
		}

		float Distance(const Vector2f other) const;
	};

	struct Rect2f;

	struct Rect2i
	{
		int x, y, w, h;

		Rect2i() = delete;

		constexpr Rect2i(const int x, const int y, const int w, const int h)
		{
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}

		constexpr Rect2i(const Vector2i position, const Vector2i size) :
			x(position.x),
			y(position.y),
			w(size.x),
			h(size.y)
		{

		}

		constexpr Rect2i operator+(const Vector2i other) const
		{
			return Rect2i
			{
				x + other.x, y + other.y,
				w, h
			};
		}

		constexpr Rect2i operator-(const Vector2i other) const
		{
			return Rect2i
			{
				x - other.x, y - other.y,
				w, h
			};
		}

		constexpr Vector2i GetPosition() const
		{
			return { x, y };
		}

		constexpr Vector2i GetSize() const
		{
			return { w, h };
		}

		Rect2f ToRect2f() const;
		RayRect2f ToRayRect2f() const;
	};

	struct Crop2f
	{
		float top = 0, bottom = 0;
		float left = 0, right = 0;

		static constexpr Crop2f GetTopCrop(const float amount)
		{
			return Crop2f{ amount, 0, 0, 0 };
		}

		static constexpr Crop2f GetBottomCrop(const float amount)
		{
			return Crop2f{ 0, amount, 0, 0 };
		}

		static constexpr Crop2f GetLeftCrop(const float amount)
		{
			return Crop2f{ 0, 0, amount, 0 };
		}

		static constexpr Crop2f GetRightCrop(const float amount)
		{
			return Crop2f{ 0, 0, 0, amount };
		}

		constexpr bool operator==(const Crop2f other) const
		{
			return
				(top == other.top) && (bottom == other.bottom) &&
				(left == other.left) && (right == other.right);
		}

		Rect2f CalculateSourceRect(const Vector2i textureSize) const;
	};

	struct Rect2f
	{
		float x = 0, y = 0, w = 0, h = 0;

		Rect2f() = delete;

		constexpr Rect2f(const float x, const float y, const float w, const float h)
		{
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}

		constexpr Rect2f(const Vector2f position, const Vector2f size) :
			x(position.x), 
			y(position.y),
			w(size.x), 
			h(size.y)
		{

		}

		static constexpr Rect2f UnitRect()
		{
			return Rect2f(0, 0, 1, 1);
		}

		constexpr bool operator==(const Rect2f other) const
		{
			const bool positionsEqual = GetPosition() == other.GetPosition();
			const bool sizesEqual = GetSize() == other.GetSize();
			return positionsEqual && sizesEqual;
		}

		constexpr bool operator!=(const Rect2f other) const
		{
			return !(operator==(other));
		}

		constexpr Vector2f GetPosition() const
		{
			return { x, y };
		}

		constexpr Vector2f GetSize() const
		{
			return { w, h };
		}

		constexpr Rect2f Translate(const Vector2f translation) const
		{
			const Vector2f originalPosition = GetPosition();
			const Vector2f dimensions = GetSize();

			const Vector2f newPosition = originalPosition + translation;
			return Rect2f(newPosition, dimensions);
		}

		constexpr Line2f GetTopLine() const
		{
			return Line2f(
				{ x - (w / 2.0f), y + (h / 2.0f) },
				{ x + (w / 2.0f), y + (h / 2.0f) }
			);
		}

		constexpr Line2f GetBottomLine() const
		{
			return Line2f(
				{ x - (w / 2.0f), y - (h / 2.0f) },
				{ x + (w / 2.0f), y - (h / 2.0f) }
			);
		}

		constexpr Line2f GetLeftLine() const
		{
			return Line2f(
				{ x - (w / 2.0f), y + (h / 2.0f) },
				{ x - (w / 2.0f), y - (h / 2.0f) }
			);
		}

		constexpr Line2f GetRightLine() const
		{
			return Line2f(
				{ x + (w / 2.0f), y + (h / 2.0f) },
				{ x + (w / 2.0f), y - (h / 2.0f) }
			);
		}

		optional<Edge2f> GetEdge(const Direction side) const;
		optional<Edge2f> GetOppositeEdge(const Direction side) const;
		optional<float> GetEdgePosition(const Direction side) const;
		optional<float> GetOppositeEdgePosition(const Direction side) const;
		optional<Line2f> GetLineBySide(const Direction side) const;

		bool CollideWithLine(const Line2f line) const;
		bool CollideWithPoint(const Vector2f point) const;

		Rect2i ToRect2i() const;

		constexpr RayRect2f ToRayRect2f() const
		{
			return RayRect2f{ x, y, w, h };
		}

		operator string() const;
	};

	struct Colour4i
	{
		byte r = 0x00, g = 0xFF, b = 0xFF, a = 0xFF;

		constexpr Colour4i()
		{
			
		}

		constexpr Colour4i(
			const byte red,
			const byte green,
			const byte blue,
			const byte alpha
		) :
			r(red), g(green), b(blue), a(alpha)
		{

		}

		constexpr Colour4i(const byte red, const byte green, const byte blue) :
			r(red), g(green), b(blue), a(0xFF)
		{

		}

		constexpr Colour4i(const byte key) :
			r(key), g(key), b(key), a(0xFF)
		{

		}

		constexpr Colour4i WithAlphaChannel(const byte newAlpha) const
		{
			return { r, g, b, newAlpha };
		}

		constexpr bool operator==(const Colour4i other) const
		{
			return
				(r == other.r) &&
				(g == other.g) &&
				(b == other.b) &&
				(a == other.a);
		}

		Colour4i WithOpacity(const float newOpacity) const;

		constexpr array<uint8_t, 4> ToArray() const
		{
			return { r, g, b, a };
		}

		constexpr RayColour ToRayColour() const
		{
			return RayColour{ r, g, b, a };
		}

		operator string() const;
	};

	struct Circle2f
	{
		Vector2f position = Vector2f::Zero();
		float radius = 1;

		Circle2f() = delete;

		constexpr Circle2f(const Vector2f position, const float radius)
		{
			this->position = position;
			this->radius = radius;
		}

		static constexpr Circle2f UnitCircle()
		{
			return Circle2f(Vector2f::Zero(), 1 / 2.0f);
		}

		constexpr Rect2f CalculateBoundingBox() const
		{
			return Rect2f(position, { radius, radius });
		}

		bool CollideWithPoint(const Vector2f point) const;
		bool CollideWithRect(const Rect2f rect) const;
	};

	constexpr Vector2f Lerp(const Vector2f a, const Vector2f b, const float t)
	{
		return { Lerp(a.x, b.x, t), Lerp(a.y, b.y, t) };
	}

	constexpr Vector2f Lerp(const Vector2i a, const Vector2f b, const float t)
	{
		return Lerp(a.ToVector2f(), b, t);
	}

	constexpr Vector2f Lerp(const Vector2f a, const Vector2i b, const float t)
	{
		return Lerp(a, b.ToVector2f(), t);
	}

	constexpr Vector2f Lerp(const Vector2i a, const Vector2i b, const float t)
	{
		return Lerp(a.ToVector2f(), b.ToVector2f(), t);
	}

	constexpr Vector2f DirectionToVector2f(const Direction direction)
	{
		switch (direction)
		{
			case Direction::Up:
				return Vector2f::Up();

			case Direction::Down:
				return Vector2f::Down();

			case Direction::Left:
				return Vector2f::Left();

			case Direction::Right:
				return Vector2f::Right();

			default:
				return Vector2f::Zero();
		}
	}

	inline static constexpr Colour4i c_White = Colour4i(0xFF);
	inline static constexpr Colour4i c_Black = Colour4i(0x00);
	inline static constexpr Colour4i c_Grey = Colour4i(0x7f);
	inline static constexpr Colour4i c_Transparent =
	{ 
		0x00, 0x00, 0x00, 0x00 
	};
}
