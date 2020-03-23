#ifndef COLLIDER_H
#define COLLIDER_H

#include "vector2.h"
#include <array>
#include <memory>

namespace Shooter {
	class LineSegmentCollider;
	class CircleCollider;
	class RectangleCollider;

	/// <summary>衝突判定をする図形全ての親クラス。図形の形状などは子クラスで定義する。</summary>
	/// <remarks>
	///     子クラスで有すメンバ変数には特に制限を設けない。すなわち、必ずしもここで定義されたangleを用いなくても良いとする。
	///     また、CheckForのrelativePositionにはColliderの中心同士の相対位置を渡す（DoesCollideWithの定義を参照）ので、
	///     それぞれのメンバ変数はoffsetの値を考慮せずに定義していることに注意。
	/// </remarks>
	class Collider
	{
	public:
		/// <param name="offset">保有先のMover::position（スプライトの中心位置）からの相対位置</param>
		/// <param name="angle">保有先のMover::angleからの相対角度</param>
		/// <remarks>2次元剛体の自由度から一意に定まる。</remarks>
		Collider(const Vector2<double>& offset, const double angle) : offset(offset), angle(angle) {}
		Collider() : offset({0.0, 0.0}), angle(0.0) {}
		virtual ~Collider() {}
		virtual bool DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const = 0;  // colliderと衝突していればtrueを返す。CheckForにたらい回す。
		virtual bool CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const = 0;  // 線分との衝突判定。
		virtual bool CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const = 0;  // 円との衝突判定。
		virtual bool CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const = 0;  // 矩形との衝突判定。

		const Vector2<double>& GetOffset() const&
		{
			return offset;
		}

		void SetOffset(const Vector2<double>& offset)
		{
			this->offset = offset;
		}

		double GetAngle() const
		{
			return angle;
		}

		virtual void SetAngle(const double angle)
		{
			this->angle = MathUtils::Modulo(angle, 2 * M_PI);  // 負の値が渡されても、0 <= angle < 2 pi になるように変換する。
		}
	protected:
		Vector2<double> offset;  // 保有先のMoverクラスのpositionを局所座標系の原点としたとき、その局所座標系における位置。
		double angle;            // 局所座標系のx軸を基準とした角度。
	};

	/// <summary>線分の衝突判定クラス。</summary>
	/// <remarks>局所座標系における原点は線分の中点とする。</remarks>
	class LineSegmentCollider : public Collider {
	public:
		/// <param name="offset">保有先のMover::position（スプライトの中心位置）からの相対位置</param>
		/// <param name="angle">保有先のMover::angleからの相対角度</param>
		/// <param name="length">線分の全長</param>
		LineSegmentCollider(const Vector2<double>& offset, const double angle, const double length)
			: Collider(offset, angle)
			, endPoints({
				MathUtils::PolarCoordinateToVector<double>(0.5e0 * length, angle + M_PI),
				MathUtils::PolarCoordinateToVector<double>(0.5e0 * length, angle)
			})
		{}

		LineSegmentCollider(const Vector2<double>& initialPoint, const Vector2<double>& terminalPoint)
			: endPoints({ initialPoint, terminalPoint })
		{
			auto orientation = terminalPoint - initialPoint;
			Collider::SetOffset(orientation * 0.5e0);
			Collider::SetAngle(std::atan2(orientation.y, orientation.x));
		}

		bool DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const override;

		double GetLength() const
		{
			return (endPoints[1] - endPoints[0]).Magnitude();
		}

		const std::array<Vector2<double>, 2>& GetEndPoints() const& {
			return endPoints;
		}

		void SetAngle(const double angle) override
		{
			for (auto&& endPoint : endPoints)
				endPoint.Rotate(-this->angle).Rotate(angle);
			Collider::SetAngle(angle);
		}
	private:
		std::array<Vector2<double>, 2> endPoints;  // 始点と終点。
	};

	/// <summary>円の衝突判定クラス。</summary>
	/// <remarks>局所座標系における原点は円の中心とする。</remarks>
	class CircleCollider : public Collider {
	public:
		/// <param name="offset">保有先のMover::position（スプライトの中心位置）からの相対位置</param>
		/// <param name="angle">保有先のMover::angleからの相対角度</param>
		/// <param name="radius">円の半径</param>
		CircleCollider(const Vector2<double>& offset, const double radius) : Collider(offset, 0.0), radius(radius) {}
		CircleCollider(const double radius) : Collider(), radius(radius) {}
		bool DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const override;

		double GetRadius() const
		{
			return radius;
		}
	private:
		double radius;
	};

	/// <summary>矩形の衝突判定クラス。</summary>
	/// <remarks>局所座標系における原点は矩形の中心とする。</remarks>
	class RectangleCollider : public Collider {
	public:
		/// <param name="offset">保有先のMover::position（スプライトの中心位置）からの相対位置</param>
		/// <param name="angle">保有先のMover::angleからの相対角度</param>
		/// <param name="size">幅と高さ</param>
		/// <remarks>sizeには特に正の実数であることを要請しない。</remarks>
		RectangleCollider(const Vector2<double>& offset, const double angle, const Vector2<double>& size)
			: Collider(offset, angle)
			, vertices({
				((-size) * 0.5e0).Rotate(angle),
				(Vector2<double>{ -size.x, size.y } * 0.5e0).Rotate(angle),
				(size * 0.5e0).Rotate(angle),
				(Vector2<double>{ size.x, -size.y } * 0.5e0).Rotate(angle),
				((-size) * 0.5e0).Rotate(angle)
			})
		{}
		RectangleCollider(const Vector2<double>& size) : RectangleCollider({ 0.e0, 0.e0 }, 0.e0, size) {}
		bool DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const override;
		bool CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const override;

		Vector2<double> GetSize() const
		{
			return { (vertices[2] - vertices[1]).Magnitude(), (vertices[0] - vertices[1]).Magnitude() };
		}

		const std::array<Vector2<double>, 5>& GetVertices() const&
		{
			return vertices;
		}

		void SetAngle(const double angle) override
		{
			for (auto&& vertex : vertices)
				vertex = vertex.Rotate(-this->angle).Rotate(angle);
			Collider::SetAngle(angle);
		}
	private:
		std::array<Vector2<double>, 5> vertices;  // 四頂点の座標。左上、左下、右下、右上の順。隣同士の差を取ると四辺のベクトルになるようにするため、1成分多い。
	};
}

#endif // !COLLIDER_H
