#ifndef VECTOR2_H
#define VECTOR2_H

// HACK: C++20からはstd::numbers::piが使える。
#define _USE_MATH_DEFINES
#include <cmath>

namespace Shooter {
	/// <summary>2次元ベクトルクラス。</summary>
	/// <remarks>代入を除いて、演算は新しくVector2かdoubleを生成する。</remarks>
	template <typename Type>
	struct Vector2
	{
		Type x, y;

		Vector2() : x(0), y(0) {}
		Vector2(const Type x, const Type y) : x(x), y(y) {}

		Vector2 &operator=(const Vector2& vector) = default;

		Vector2 operator+(const Vector2& vector) const
		{
			return { this->x + vector.x, this->y + vector.y };
		}

		Vector2 operator-(const Vector2& vector) const
		{
			return { this->x - vector.x, this->y - vector.y };
		}

		// TODO: 順序不問にする。
		Vector2 operator*(const Type& scalar) const
		{
			return { this->x * scalar, this->y * scalar };
		}

		Vector2 operator/(const Type& scalar) const
		{
			return { this->x / scalar, this->y / scalar };
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

		bool operator==(const Vector2& vector) const
		{
			return (this->x == vector.x) && (this->y == vector.y);
		}

		Type Magnitude() const
		{
			return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
		}

		Type SquaredMagnitude() const
		{
			return std::pow(x, 2) + std::pow(y, 2);
		}

		Vector2 Normalize() const
		{
			if (x != 0.0 || y != 0.0)
				return { x / Magnitude(), y / Magnitude() };
			else
				return { 0.0, 0.0 };
		}

		Type Dot(const Vector2& vector) const
		{
			return this->x * vector.x + this->y * vector.y;
		}

		Type Cross(const Vector2& vector) const
		{
			return this->x * vector.y - this->y * vector.x;
		}

		/// <summary>ベクトルを回転させる。</summary>
		/// <param name="angle">x軸からy軸に向かう方向を正とする角度</angle>
		/// <remarks>実数型でないと縮小変換。</remarks>
		Vector2 Rotate(const Type angle) const
		{
			return { std::cos(angle) * x - std::sin(angle) * y, std::sin(angle) * x + std::cos(angle) * y };
		}
	};

	/// <summary>矩形クラス。</summary>
	/// <remarks>画像の切り抜きなどに利用する。</remarks>
	template <typename Type>
	struct Rect
	{
		Type x, y;           // 左上端の座標。
		Type width, height;  // 幅、高さ。

		Rect() : x(0), y(0), width(0), height(0) {}
		Rect(const Type x, const Type y, const Type width, const Type height) : x(x), y(y), width(width), height(height) {}
		Rect(const Vector2<Type>& position, const Vector2<Type>& size) : x(position.x), y(position.y), width(size.x), height(size.y) {}
	};

	namespace MathUtils {
		/// <summary>剰余を求める。返り値は被除数と同じ符号。</summary>
		inline int Remainder(const int Dividend, const int Divisor)
		{
			return (Dividend - Divisor * static_cast<int>(std::trunc(static_cast<double>(Dividend) / Divisor)));
		}

		inline double Remainder(const double Dividend, const double Divisor)
		{
			return (Dividend - Divisor * std::trunc(Dividend / Divisor));
		}

		/// <summary>剰余を求める。返り値は除数と同じ符号。</summary>
		inline int Modulo(const int Dividend, const int Divisor)
		{
			return (Dividend - Divisor * static_cast<int>(std::floor(static_cast<double>(Dividend) / Divisor)));
		}

		inline double Modulo(const double Dividend, const double Divisor)
		{
			return (Dividend - Divisor * std::floor(Dividend / Divisor));
		}

		/// <summary>極座標から（Cartesian座標の）ベクトルを生成する。</summary>
		template<typename Type>
		inline Vector2<Type> PolarCoordinateToVector(const Type radius, const Type angle)
		{
			return Vector2<Type>{ std::cos(angle), std::sin(angle) } * radius;
		}

		/// <summary>Vector2<double>からVector2<int>を生成する。Ceil関数で丸める。</summary>
		inline Vector2<int> CeilToInt(const Vector2<double>& vector)
		{
			return { static_cast<int>(std::ceil(vector.x)), static_cast<int>(std::ceil(vector.y)) };
		}

		/// <summary>Vector2<double>からVector2<int>を生成する。Floor関数で丸める。</summary>
		inline Vector2<int> FloorToInt(const Vector2<double>& vector)
		{
			return { static_cast<int>(std::floor(vector.x)), static_cast<int>(std::floor(vector.y)) };
		}

		/// <summary>Vector2<double>からVector2<int>を生成する。Round関数で丸める。</summary>
		inline Vector2<int> RoundToInt(const Vector2<double>& vector)
		{
			return { static_cast<int>(std::round(vector.x)), static_cast<int>(std::round(vector.y)) };
		}
	}
}

#endif // !VECTOR2_H
