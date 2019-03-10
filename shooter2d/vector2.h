﻿#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

namespace Shooter {
	struct Vector2
	{
		float x, y;

		Vector2(float x, float y) : x(x), y(y) {}

		Vector2 &operator=(const Vector2 &vector) = default;

		Vector2 operator+(const Vector2 &vector) const
		{
			return { this->x + vector.x, this->y + vector.y };
		}

		Vector2 operator-(const Vector2 &vector) const
		{
			return { this->x - vector.x, this->y + vector.y };
		}

		Vector2 operator*(const float &scalar) const
		{
			return { this->x * scalar, this->y * scalar };
		}

		Vector2 &operator+=(const Vector2 &vector)
		{
			x += vector.x;
			y += vector.y;
			return *this;
		}

		float Magnitude()
		{
			return std::sqrt(std::pow(x, 2) + std::pow(y, 2));
		}

		Vector2 Normalize()
		{
			if (x != 0.0f || y != 0.0f)
				return { x / Magnitude(), y / Magnitude() };
			else
				return { 0.0f, 0.0f };
		}
	};
}

#endif // !VECTOR2_H