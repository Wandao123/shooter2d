#ifndef COLLIDER_H
#define COLLIDER_H

#include "vector2.h"
#include <memory>

namespace Shooter {
	class LineSegmentCollider;
	class CircleCollider;
	class RectangleCollider;

	class Collider
	{
	public:
		/// <param name="offset">保有先のMover::position（スプライトの中心位置）からの相対位置</param>
		/// <param name="angle">保有先のMover::angleからの相対角度</param>
		Collider(const Vector2<double>& offset, const double angle) : offset(offset), angle(angle) {}
		Collider() : offset({0.0, 0.0}), angle(0.0) {}
		virtual ~Collider() {}
		virtual bool DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const = 0;  // colliderと衝突していればtrueを返す。CheckForにたらい回す。
		virtual bool CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const = 0;  // 線分との衝突判定。
		virtual bool CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const = 0;  // 円との衝突判定。
		virtual bool CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const = 0;  // 矩形との衝突判定。
		const Vector2<double>& GetOffset() const&;
		void SetOffset(const Vector2<double>& offset);
		double GetAngle() const;
		void SetAngle(const double angle);
	protected:
		Vector2<double> offset;  // 保有先のMoverクラスのpositionを局所座標系の原点としたとき、その局所座標系における位置。
		double angle;  // HACK: 傾き具合を定義するより、各クラスそれぞれに応じてベクトルを使う方がスマートかもしれない。
	};

	// 局所座標系における原点は線分の中点とする。
	class LineSegmentCollider : public Collider {
	public:
		//LineSegmentCollider(const Vector2<double>& initialPoint, const Vector2<double>& terminalPoint)
		//	: Collider(initialPoint, 0.0), orientation(terminalPoint - initialPoint) {}
		LineSegmentCollider(const Vector2<double>& offset, const double angle, const double length)
			: Collider(offset, angle), length(length)
		{}
		bool DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const override;
		//const Vector2<double>& GetOrientation() const&;
		//Vector2<double> GetTerminalPoint() const;
		double GetLength() const;
	private:
		//Vector2<double> orientation;  // 方向ベクトル。
		double length;  // 線分の全長。
	};

	// 局所座標系における原点は円の中心とする。
	class CircleCollider : public Collider {
	public:
		CircleCollider(const Vector2<double>& offset, const double radius) : Collider(offset, 0.0), radius(radius) {}
		CircleCollider(const double radius) : Collider(), radius(radius) {}
		bool DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const override;
		double GetRadius() const;
	private:
		double radius;
	};

	// 局所座標系における原点は矩形の中心とする。
	class RectangleCollider : public Collider {
	public:
		RectangleCollider(const Vector2<double>& offset, const double angle, const double height, const double width)
			: Collider(offset, angle), height(height), width(width)
		{}
		RectangleCollider(const double height, const double width) : Collider(), height(height), width(width) {}
		bool DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const override;
		double GetHeight() const;
		double GetWidth() const;
	private:
		double height;
		double width;
	};
}

#endif // !COLLIDER_H
