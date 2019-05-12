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
		/// <param name="position">保有先のMoverからの相対位置</param>
		Collider(const Vector2& position) : position(position) {}
		
		virtual ~Collider() {}
		virtual bool DoesCollideWith(const Vector2& relativeVector, Collider& collider) = 0;  // colliderと衝突していればtrueを返す。CheckForにたらい回す。
		virtual bool CheckFor(const Vector2& relativeVector, const CircleCollider& circle) = 0;        // 円との衝突判定。
		virtual bool CheckFor(const Vector2& relativeVector, const RectangleCollider& rectangle) = 0;  // 矩形との衝突判定。

		Vector2 GetPosition() const
		{
			return position;
		}

		void SetPosition(const Vector2& position)
		{
			this->position = position;
		}
	protected:
		Vector2 position;  // ここでいう位置とは、保有先のMoverクラスのpositionを局所座標系の原点としたとき、その局所座標系における位置。
	};

	// 円のpositionとは円の中心。
	class CircleCollider : public Collider {
	public:
		CircleCollider(const Vector2& position, const float radius) : Collider(position), radius(radius) {}
		bool DoesCollideWith(const Vector2& relativeVector, Collider& collider) override;
		bool CheckFor(const Vector2& relativeVector, const CircleCollider& circle) override;
		bool CheckFor(const Vector2& relativeVector, const RectangleCollider& rectangle) override;
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
		bool DoesCollideWith(const Vector2& relativeVector, Collider& collider) override;
		bool CheckFor(const Vector2& relativeVector, const CircleCollider& circle) override;
		bool CheckFor(const Vector2& relativeVector, const RectangleCollider& rectangle) override;
	private:
		float height;
		float width;
	};
}

#endif // !COLLIDER_H