#include "collider.h"

namespace Shooter {
	/// <param name="relativeVector">全体座標系における保有先のMover同士の相対ベクトル</param>
	/// <param name="collider">衝突判定する対象</param>
	bool CircleCollider::DoesCollideWith(const Vector2& relativeVector, Collider& collider)
	{
		return collider.CheckFor(relativeVector, *this);
	}

	bool CircleCollider::CheckFor(const Vector2& relativeVector, const CircleCollider& circle)
	{
		float squiredDistance = (relativeVector + circle.GetPosition() - position).SquiredMagnitude();
		if (squiredDistance <= std::powf(circle.GetRadius() + radius, 2.0f))
			return true;
		else
			return false;
	}

	bool CircleCollider::CheckFor(const Vector2& relativeVector, const RectangleCollider& rectangle)
	{
		// TODO: 実装方法？
		return false;
	}

	float CircleCollider::GetRadius() const
	{
		return radius;
	}

	/// <param name="relativeVector">全体座標系におけるcolliderまでの相対ベクトル</param>
	/// <param name="collider">衝突判定する対象</param>
	bool RectangleCollider::DoesCollideWith(const Vector2& relativeVector, Collider& collider)
	{
		return collider.CheckFor(relativeVector, *this);
	}

	bool RectangleCollider::CheckFor(const Vector2& relativeVector, const CircleCollider& circle)
	{
		// TODO: 実装方法？
		return false;
	}

	bool RectangleCollider::CheckFor(const Vector2& relativeVector, const RectangleCollider& rectangle)
	{
		// TODO: 回転した場合？
		return false;
	}
}