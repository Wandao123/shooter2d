#include "collider.h"

namespace Shooter {
	bool CircleCollider::DoesCollideWith(Collider& collider)
	{
		return collider.CheckFor(*this);
	}

	bool CircleCollider::CheckFor(const CircleCollider& circle)
	{
		float distance = std::powf(circle.GetPosition().x - position.x, 2.0f)
			+ std::powf(circle.GetPosition().y - position.y, 2.0f);
		if (distance <= std::powf(circle.GetRadius() + radius, 2.0f))
			return true;
		else
			return false;
	}

	bool CircleCollider::CheckFor(const RectangleCollider& rectangle)
	{
		// TODO: 実装方法？
		return false;
	}

	float CircleCollider::GetRadius() const
	{
		return radius;
	}

	bool RectangleCollider::DoesCollideWith(Collider& collider)
	{
		return collider.CheckFor(*this);
	}

	bool RectangleCollider::CheckFor(const CircleCollider& circle)
	{
		// TODO: 実装方法？
		return false;
	}

	bool RectangleCollider::CheckFor(const RectangleCollider& rectangle)
	{
		// TODO: 回転した場合？
		return false;
	}
}