#ifndef VECTOR2_H
#define VECTOR2_H

#include <cmath>

namespace Shooter {
	struct Vector2
	{
		float x, y;

		Vector2(float x, float y) : x(x), y(y) {}

		Vector2 &operator=(const Vector2 &vector) = default;

		//Vector2 Add(Vector2 vector)
		Vector2 operator+(const Vector2 &vector) const
		{
			return { this->x + vector.x, this->y + vector.y };
		}

		//Vector2 Subtract(Vector2 vector)
		Vector2 operator-(const Vector2 &vector) const
		{
			return { this->x - vector.x, this->y + vector.y };
		}

		//Vector2 ScalarMultiply(float scalar)
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
			//return vector.ScalarMultiply(1.0f / vector.Magnitude());
			return { x / Magnitude(), y / Magnitude() };
		}
	};
}

#endif // !VECTOR2_H
