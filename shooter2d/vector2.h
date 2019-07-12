#ifndef VECTOR2_H
#define VECTOR2_H

#define _USE_MATH_DEFINES
#include <cmath>

namespace Shooter {
	/// <summary>2次元ベクトルクラス。</summary>
	/// <remarks>代入を除いて、演算は新しくVector2かfloatを生成する。</remarks>
	struct Vector2
	{
		float x, y;

		Vector2(float x, float y) : x(x), y(y) {}

		Vector2 &operator=(const Vector2& vector) = default;

		Vector2 operator+(const Vector2& vector) const
		{
			return { this->x + vector.x, this->y + vector.y };
		}

		Vector2 operator-(const Vector2& vector) const
		{
			return { this->x - vector.x, this->y - vector.y };
		}

		Vector2 operator*(const float& scalar) const
		{
			return { this->x * scalar, this->y * scalar };
		}

		Vector2 operator-() const
		{
			return { -this->x, -this->y };
		}

		Vector2 &operator+=(const Vector2& vector)
		{
			x += vector.x;
			y += vector.y;
			return *this;
		}

		float Magnitude() const
		{
			return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
		}

		float SquaredMagnitude() const
		{
			return std::pow(x, 2) + std::pow(y, 2);
		}

		Vector2 Normalize() const
		{
			if (x != 0.0f || y != 0.0f)
				return { x / Magnitude(), y / Magnitude() };
			else
				return { 0.0f, 0.0f };
		}

		float Dot(const Vector2& vector) const
		{
			return this->x * vector.x + this->y * vector.y;
		}

		float Cross(const Vector2& vector) const
		{
			return this->x * vector.y - this->y * vector.x;
		}

		Vector2 Rotate(const float angle) const
		{
			return { std::cos(angle) * x - std::sin(angle) * y, std::sin(angle) * x + std::cos(angle) * y };
		}
	};

	namespace MathUtils {
		/// <summary>剰余を求める。返り値は被除数と同じ符号。</summary>
		inline int Remainder(const int Dividend, const int Divisor)
		{
			return (Dividend - Divisor * std::trunc(static_cast<double>(Dividend) / Divisor));
		}

		inline float Remainder(const float Dividend, const float Divisor)
		{
			return (Dividend - Divisor * std::trunc(Dividend / Divisor));
		}

		/// <summary>剰余を求める。返り値は除数と同じ符号。</summary>
		inline int Modulo(const int Dividend, const int Divisor)
		{
			return (Dividend - Divisor * std::floor(static_cast<double>(Dividend) / Divisor));
		}

		inline float Modulo(const float Dividend, const float Divisor)
		{
			return (Dividend - Divisor * std::floor(Dividend / Divisor));
		}
	}
}

#endif // !VECTOR2_H
