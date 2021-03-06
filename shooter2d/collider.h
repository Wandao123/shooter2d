#ifndef COLLIDER_H
#define COLLIDER_H

#include "vector2.h"
#include <memory>

namespace Shooter {
	class CircleCollider;
	class RectangleCollider;

	class Collider
	{
	public:
		/// <param name="position">保有先のMover::position（スプライトの中心位置）からの相対位置</param>
		/// <param name="angle">保有先のMover::angleからの相対角度</param>
		Collider(const Vector2<double>& position, const double angle) : position(position), angle(angle) {}
		Collider() : position({0.0, 0.0}), angle(0.0) {}
		virtual ~Collider() {}
		virtual bool DoesCollideWith(const Vector2<double>& relativeVector, Collider& collider) const = 0;  // colliderと衝突していればtrueを返す。CheckForにたらい回す。
		virtual bool CheckFor(const Vector2<double>& relativeVector, const CircleCollider& circle) const = 0;        // 円との衝突判定。
		virtual bool CheckFor(const Vector2<double>& relativeVector, const RectangleCollider& rectangle) const = 0;  // 矩形との衝突判定。
		Vector2<double> GetPosition() const;
		void SetPosition(const Vector2<double>& position);
		double GetAngle() const;
		void SetAngle(const double angle);
	protected:
		Vector2<double> position;  // ここでいう位置とは、保有先のMoverクラスのpositionを局所座標系の原点としたとき、その局所座標系における位置。
		double angle;
	};

	// 円のpositionとは円の中心。
	class CircleCollider : public Collider {
	public:
		CircleCollider(const Vector2<double>& position, const double radius) : Collider(position, 0.0), radius(radius) {}
		CircleCollider(const double radius) : Collider(), radius(radius) {}
		bool DoesCollideWith(const Vector2<double>& relativeVector, Collider& collider) const override;
		bool CheckFor(const Vector2<double>& relativeVector, const CircleCollider& circle) const override;
		bool CheckFor(const Vector2<double>& relativeVector, const RectangleCollider& rectangle) const override;
		double GetRadius() const;
	private:
		double radius;
	};

	// 矩形positionとは矩形の中心。
	class RectangleCollider : public Collider {
	public:
		RectangleCollider(const Vector2<double>& position, const double angle, const double height, const double width)
			: Collider(position, angle), height(height), width(width)
		{}
		RectangleCollider(const double height, const double width) : Collider(), height(height), width(width) {}
		bool DoesCollideWith(const Vector2<double>& relativeVector, Collider& collider) const override;
		bool CheckFor(const Vector2<double>& relativeVector, const CircleCollider& circle) const override;
		bool CheckFor(const Vector2<double>& relativeVector, const RectangleCollider& rectangle) const override;
		double GetHeight() const;
		double GetWidth() const;
	private:
		double height;
		double width;
	};
}

#endif // !COLLIDER_H
