#ifndef COLLIDER_H
#define COLLIDER_H

#include <memory>
#include "vector2.h"

namespace Shooter {
	class CircleCollider;
	class RectangleCollider;

	class Collider
	{
	public:
		Collider(const Vector2& position) : position(position) {}
		~Collider() = default;
		virtual bool DoesCollideWith(Collider& collider) = 0;           // colliderと衝突していればtrueを返す。CheckForにたらい回しにする。
		virtual bool CheckFor(const CircleCollider& circle) = 0;        // 円との衝突判定。
		virtual bool CheckFor(const RectangleCollider& rectangle) = 0;  // 矩形との衝突判定。

		Vector2 GetPosition() const
		{
			return position;
		}

		void SetPosition(const Vector2& position)
		{
			this->position = position;
		}
	protected:
		Vector2 position;
	};

	// 円のpositionとは円の中心。
	class CircleCollider : public Collider {
	public:
		CircleCollider(const Vector2& position, const float radius) : Collider(position), radius(radius) {}
		bool DoesCollideWith(Collider& collider) override;
		bool CheckFor(const CircleCollider& circle) override;
		bool CheckFor(const RectangleCollider& rectangle) override;
		float GetRadius() const;
	private:
		float radius;
	};

	// 矩形positionとは矩形の左上端。
	class RectangleCollider : public Collider {
	public:
		RectangleCollider(const Vector2& position, const float height, const float width)
			: Collider(position), height(height), width(width)
		{}
		bool DoesCollideWith(Collider& collider) override;
		bool CheckFor(const CircleCollider& circle) override;
		bool CheckFor(const RectangleCollider& rectangle) override;
	private:
		float height;
		float width;
	};
}

#endif // !COLLIDER_H