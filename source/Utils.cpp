#include "Utils.h"

namespace BlastOff
{
	optional<float> Edge2f::GetSign() const
	{
		switch (side)
		{
		case Direction::Up:
		case Direction::Right:
			return +1.0f;

		case Direction::Down:
		case Direction::Left:
			return -1.0f;

		default:
			return std::nullopt;
		}
	}


	optional<float> Vector2f::Angle(const bool useRadians) const
	{
		if ((*this) == Vector2f::Zero())
			return std::nullopt;

		const float unconverted = atan2f(x, y);
		if (useRadians)
			return unconverted;
		else
			return ToDegrees(unconverted);
	}

	Vector2f Vector2f::Rotate(const float angle, const bool useRadians) const
	{
		const auto calculateRadians =
			[&]() -> float
			{
				if (!useRadians)
					return ToRadians(angle);
				else
					return angle;
			};

		const float convertedAngle = calculateRadians();
		
		const float sinTheta = sinf(convertedAngle);
		const float cosTheta = cosf(convertedAngle);
		
		const Vector2f result =
		{
			-((x * cosTheta) - (y * sinTheta)),
			(x * sinTheta) + (y * cosTheta)
		};
		return result;
	}

	Vector2f Vector2f::Normalize() const
	{
		return (*this) / Magnitude();
	}

	float Vector2f::Distance(const Vector2f other) const
	{
		return ((*this) - other).Magnitude();
	}

	float Vector2f::Magnitude() const
	{
		return sqrtf(x * x + y * y);
	}

	Vector2i Vector2f::ToVector2i() const
	{
		return Vector2i{ (int)roundf(x), (int)roundf(y) };
	}

	Vector2i Vector2f::CeilToVector2i() const
	{
		return Vector2i{ (int)ceilf(x), (int)ceilf(y) };
	}

	Vector2f Vector2f::RoundToFraction(const float fraction) const
	{
		return ((*this) / fraction).ToVector2i() * fraction;
	}

	Vector2f::operator string() const
	{
		const string membersString =
			std::to_string(x) + ", " +
			std::to_string(y);

		return "Vector2f(" + membersString + ")";
	}


	Vector2i FromRayVector2f(const RayVector2f value)
	{
		return Vector2i{ (int)roundf(value.x), (int)roundf(value.y) };
	}

	Vector2f Vector2i::Normalize() const
	{
		return (*this) / Magnitude();
	}

	float Vector2i::Magnitude() const
	{
		const auto convX = (float)x;
		const auto convY = (float)y;

		return sqrtf(convX * convX + convY * convY);
	}

	Vector2i::operator string() const
	{
		const string membersString =
			std::to_string(x) + ", " +
			std::to_string(y);

		return "Vector2i(" + membersString + ")";
	}


	optional<Edge2f> Rect2f::GetEdge(
		const Direction side, 
		const bool useCenterOrigin
	) const
	{
		const auto getWithCenterOrigin =
			[&, this]() -> optional<Edge2f>
			{
				switch (side)
				{
				case Direction::Up:
					return Edge2f{ side, y + (h / 2.0f) };

				case Direction::Down:
					return Edge2f{ side, y - (h / 2.0f) };

				case Direction::Left:
					return Edge2f{ side, x - (w / 2.0f) };

				case Direction::Right:
					return Edge2f{ side, x + (w / 2.0f) };

				default:
					return std::nullopt;
				}
			};

		if (useCenterOrigin)
			return getWithCenterOrigin();
		else
		{
			const char* const message =
			{
				"Rect2f::GetEdge(..., useCenterOrigin = false) "
				"not implemented"
			};
			throw std::runtime_error(message);
		}
	}

	optional<Edge2f> Rect2f::GetOppositeEdge(
		const Direction side,
		const bool useCenterOrigin
	) const
	{
		const optional<Direction> oppositeSide = GetOppositeDirection(side);
		if (!oppositeSide)
			return std::nullopt;
		else
			return GetEdge(*oppositeSide, useCenterOrigin);
	}

	optional<float> Rect2f::GetEdgePosition(
		const Direction side,
		const bool useCenterOrigin 
	) const
	{
		const optional<Edge2f> edge = GetEdge(side, useCenterOrigin);
		if (edge)
			return edge->position;
		else
			return std::nullopt;
	}

	optional<float> Rect2f::GetOppositeEdgePosition(
		const Direction side,
		const bool useCenterOrigin
	) const
	{
		const optional<Edge2f> edge = GetOppositeEdge(side, useCenterOrigin);
		if (edge)
			return edge->position;
		else
			return std::nullopt;
	}

	optional<Line2f> Rect2f::GetLineBySide(
		const Direction side,
		const bool useCenterOrigin
	) const
	{
		const auto getWithCenterOrigin =
			[&, this]()->optional<Line2f>
			{
				switch (side)
				{
				case Direction::Up:
					return GetTopLine();

				case Direction::Down:
					return GetBottomLine();

				case Direction::Left:
					return GetLeftLine();

				case Direction::Right:
					return GetRightLine();

				default:
					return std::nullopt;
				}
			};

		if (useCenterOrigin)
			return getWithCenterOrigin();
		else
		{
			const char* const message =
			{
				"Rect2f::GetLineBySide(..., useCenterOrigin = false) "
				"not implemented"
			};
			throw std::runtime_error(message);
		}
	}

	bool Rect2f::CollideWithLine(const Line2f line) const
	{
		for (const Direction side : GetDirectionsWithoutNone())
		{
			const optional<Line2f> sideLine = GetLineBySide(side);
			if (!sideLine)
			{
				const char* const message =
				{
					"Rect2f::GetLineBySide() failed."
				};
				throw std::runtime_error(message);
			}

			const bool collision = line.Collide(*sideLine);
			if (collision)
				return true;
		}
		return false;
	}

	bool Rect2f::CollideWithPoint(const Vector2f point) const
	{
		const optional<float> topEdge = GetEdgePosition(Direction::Up);
		const optional<float> bottomEdge = GetEdgePosition(Direction::Down);
		const optional<float> leftEdge = GetEdgePosition(Direction::Left);
		const optional<float> rightEdge = GetEdgePosition(Direction::Right);

		const auto throwEdgeException =
			[&](const Direction side)
			{
				const string errorHeader =
				{
					"Rect2f::CollideWithPoint(Vector2f) failed: "
				};
				const string message =
				{
					errorHeader +
					"GetEdgePosition(" + DirectionToString(side) + ") failed."
				};
				throw std::runtime_error(message);
			};

		if (!topEdge)
		{
			throwEdgeException(Direction::Up);
			return false;
		}
		if (!bottomEdge)
		{
			throwEdgeException(Direction::Down);
			return false;
		}
		if (!leftEdge)
		{
			throwEdgeException(Direction::Left);
			return false;
		}
		if (!rightEdge)
		{
			throwEdgeException(Direction::Right);
			return false;
		}

		return
			(point.y > (*bottomEdge)) &&
			(point.y < (*topEdge)) &&
			(point.x > (*leftEdge)) &&
			(point.x < (*rightEdge));
	}

	Rect2i Rect2f::ToRect2i() const
	{
		return Rect2i
		{
			(int)roundf(x),
			(int)roundf(y),
			(int)roundf(w),
			(int)roundf(h)
		};
	}

	Rect2f::operator string() const
	{
		const string membersString = 
			std::to_string(x) + ", " + std::to_string(y) + ", " +
			std::to_string(w) + ", " + std::to_string(h);

		return "Rect2f(" + membersString + ")";
	}


	Rect2f Rect2i::ToRect2f() const
	{
		return Rect2f{ (float)x, (float)y, (float)w, (float)h };
	}

	RayRect2f Rect2i::ToRayRect2f() const
	{
		return RayRect2f{ (float)x, (float)y, (float)w, (float)h };
	}


	bool Circle2f::CollideWithPoint(const Vector2f point) const
	{
		const float distance = position.Distance(point);
		return distance <= radius;
	}

	bool Circle2f::CollideWithRect(const Rect2f rect) const
	{
		const float xDistance = fabsf(position.x - rect.x);
		const float yDistance = fabsf(position.y - rect.y);

		const float xBoundary = (rect.w / 2.0f) + radius;
		if (xDistance > xBoundary)
			return false;

		const float yBoundary = (rect.h / 2.0f) + radius;
		if (yDistance > yBoundary)
			return false;

		if (xDistance <= (rect.w / 2.0f))
			return true;
		if (yDistance <= (rect.h / 2.0f))
			return true;

		const Vector2f squaredCornerDistances =
		{
			powf(xDistance - (rect.w / 2.0f), 2),
			powf(yDistance - (rect.h / 2.0f), 2)
		};
		const Vector2f sCD = squaredCornerDistances;
		const float addedDistances = sCD.x + sCD.y;

		const bool result = addedDistances <= powf(radius, 2);
		return result;
	}


	float Line2f::Distance(const Vector2f other) const
	{
		const Vector2f centerPosition = CenterPosition();
		return centerPosition.Distance(other);
	}


	Colour4i Colour4i::WithOpacity(const float newOpacity) const
	{
		const byte newAlpha = (byte)roundf(newOpacity * 0xFF);
		return WithAlphaChannel(newAlpha);
	}

	Colour4i::operator string() const
	{
		const auto convert = ByteToHexString;
		const string channelsString =
			convert(r) + ", " + convert(g) + ", " +
			convert(b) + ", " + convert(a);

		return "Colour4i(" + channelsString + ")";
	}


	string ByteToHexString(const byte value)
	{
		return std::format("{:#04x}", value);
	}

	string BoolToString(const bool value)
	{
		return value ? "true" : "false";
	}

	float GetRandomFloat()
	{
		constexpr int max = std::numeric_limits<int>::max();
		return GetRandomValue(0, max) / (float)max;
	}


	// TODO: try caching expensive method
	Rect2f Crop2f::CalculateSourceRect(const Vector2i textureSize) const
	{
		if ((*this) == Crop2f{ 0 })
			return Rect2f(Vector2f::Zero(), textureSize.ToVector2f());
		else
		{
			const float x = left * textureSize.x;
			const float y = top * textureSize.y;
			const float w = ((1 - right) * textureSize.x) - x;
			const float h = ((1 - bottom) * textureSize.y) - y;

			return Rect2f({ x, y }, { w, h });
		}
	}


	float SineInterpolation(const float number)
	{
		// link to graph:
		// https://www.desmos.com/calculator/ticikc609f
		constexpr float A = -1 / 2.0;
		constexpr float B = 3 / 2.0;

		return A * (sinf(c_Pi * (number - B)) - 1);
	}

	float NthSineInterpolation(
		const float number,
		const float applications
	)
	{
		const float last_index = floor(applications);
		float result = number;

		for (size_t index = 0; index < last_index; index++)
			result = SineInterpolation(result);

		// interpolate if 'applications' is a non-integer number
		if ((applications - last_index) > 0)
		{
			const float next_application = SineInterpolation(result);
			const float progress = 1 - (applications - last_index);
			if (next_application > result)
				result = Lerp(result, next_application, progress);
			else
				result = Lerp(next_application, result, progress);
		}
		return result;
	}

	float DoubleSineInterpolation(const float number)
	{
		return NthSineInterpolation(number, 2);
	}
}
