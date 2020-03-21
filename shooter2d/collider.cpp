#include "collider.h"

using namespace Shooter;

/******************************** LineSegmentCollider *********************************/

/// <summary>受け取ったColliderクラスのCheckForを呼び出す。</summary>
/// <param name="relativePosition">保有先のMover同士の相対位置ベクトル</param>
/// <param name="collider">衝突判定する対象</param>
/// <remarks>relativePositionはcolliderの保有先Mover（相手）を基準点とする。</remarks>
bool LineSegmentCollider::DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const
{
	return collider.CheckFor(offset + relativePosition - collider.GetOffset(), *this);
}

/// <summary>LineSegmentColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="lineSegment">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool LineSegmentCollider::CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const
{
	const double EPS = 1.e-8;
	auto orientation1 = this->endPoints[1] - this->endPoints[0];
	auto orientation2 = relativePosition + lineSegment.endPoints[1] - lineSegment.endPoints[0];
	auto differenceBetweenInitialPoints = lineSegment.endPoints[0] - this->endPoints[0];
	if (orientation1.Cross(orientation2) < EPS)
		return false;
	if (differenceBetweenInitialPoints.Cross(orientation2) / orientation1.Cross(orientation2) >= 0.e0
			&& differenceBetweenInitialPoints.Cross(orientation2) / orientation1.Cross(orientation2) <= 1.e0
			&& differenceBetweenInitialPoints.Cross(orientation1) / orientation1.Cross(orientation2) >= 0.e0
			&& differenceBetweenInitialPoints.Cross(orientation1) / orientation1.Cross(orientation2) <= 1.e0)
		return true;
	return false;
}

/// <summary>CircleColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="circle">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool LineSegmentCollider::CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const
{
	auto orientation = endPoints[1] - endPoints[0];
	if (std::abs(orientation.Normalize().Cross(relativePosition)) > circle.GetRadius())
		return false;
	if ((relativePosition + orientation * 0.5e0).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.e0)
			|| (relativePosition - orientation * 0.5e0).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.e0))
		return true;
	if ((relativePosition + orientation * 0.5e0).Dot(orientation) * (relativePosition + orientation * 0.5e0).Dot(orientation) <= 0.e0)
		return true;
	return false;
}

/// <summary>RectangleColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="rectangle">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool LineSegmentCollider::CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const
{
	auto endPoints = GetEndPoints();
	auto vertices = rectangle.GetVertices();

	// 線分の端点が矩形に含まれる場合。
	for (std::size_t i = 0; i < endPoints.size(); i++) {
		auto result = true;
		for (std::size_t j = 1; j < vertices.size(); j++)
			result = result && ((vertices[j] - vertices[j - 1]).Cross(endPoints[i] - vertices[j - 1]) <= 0.e0);
		if (result)
			return true;
	}

	// 線分と矩形の辺とが交差する場合。
	for (std::size_t i = 1; i < vertices.size(); i++) {
		auto diff = vertices[i] - vertices[i - 1];
		auto segment = LineSegmentCollider({ 0.e0, 0.e0 }, std::atan2(diff.y, diff.x), diff.Magnitude());
		if (CheckFor(relativePosition + (vertices[i] + vertices[i - 1]) * 0.5e0, segment))
			return true;
	}
	return false;
}

/******************************** CircleCollider *********************************/

/// <summary>受け取ったColliderクラスのCheckForを呼び出す。</summary>
/// <param name="relativePosition">保有先のMover同士の相対位置ベクトル</param>
/// <param name="collider">衝突判定する対象</param>
/// <remarks>relativePositionはcolliderの保有先Mover（相手）を基準点とする。</remarks>
bool CircleCollider::DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const
{
	return collider.CheckFor(offset + relativePosition - collider.GetOffset(), *this);
}

/// <summary>LineSegmentColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="lineSegment">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool CircleCollider::CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const
{
	auto direction = MathUtils::PolarCoordinateToVector<double>(lineSegment.GetLength(), lineSegment.GetAngle());
	if (std::abs(direction.Normalize().Cross(relativePosition)) > radius)
		return false;
	if ((relativePosition + direction * 0.5e0).SquaredMagnitude() < std::pow(radius, 2.e0)
			|| (relativePosition - direction * 0.5e0).SquaredMagnitude() < std::pow(radius, 2.e0))
		return true;
	if ((relativePosition + direction * 0.5e0).Dot(direction) * ((relativePosition - direction * 0.5e0)).Dot(direction) <= 0.e0)
		return true;
	return false;
}

/// <summary>CircleColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="circle">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool CircleCollider::CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const
{
	if (relativePosition.SquaredMagnitude() <= std::pow(circle.GetRadius() + radius, 2.0))
		return true;
	else
		return false;
}

/// <summary>RectangleColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="rectangle">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool CircleCollider::CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const
{
	// 矩形の中心を座標系の原点に取る。また、矩形が各座標軸と平行になるように回転させる。
	auto circPos = (-relativePosition).Rotate(-rectangle.GetAngle());
	auto size = rectangle.GetSize();
	if (circPos.x > -size.x * 0.5e0 && circPos.x < size.x * 0.5e0
			&& circPos.y > -size.y * 0.5e0 - radius && circPos.y < size.y * 0.5e0 + radius)
		return true;
	else if (circPos.x > -size.x * 0.5e0 - radius && circPos.x < size.x * 0.5e0 + radius
			&& circPos.y > -size.y * 0.5e0 && circPos.y < size.y * 0.5e0)
		return true;
	auto vertices = rectangle.GetVertices();
	for (auto&& vertex : vertices)
		if (vertex.SquaredMagnitude() < std::pow(radius, 2.e0))
			return true;
	return false;
}

/******************************** RectangleCollider *********************************/

/// <summary>受け取ったColliderクラスのCheckForを呼び出す。</summary>
/// <param name="relativePosition">保有先のMover同士の相対位置ベクトル</param>
/// <param name="collider">衝突判定する対象</param>
/// <remarks>relativePositionはcolliderの保有先Mover（相手）を基準点とする。</remarks>
bool RectangleCollider::DoesCollideWith(const Vector2<double>& relativePosition, Collider& collider) const
{
	return collider.CheckFor(offset + relativePosition - collider.GetOffset(), *this);
}

/// <summary>LineSegmentColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="lineSegment">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool RectangleCollider::CheckFor(const Vector2<double>& relativePosition, const LineSegmentCollider& lineSegment) const
{
	auto endPoints = lineSegment.GetEndPoints();

	// 線分の端点が矩形に含まれる場合。
	for (std::size_t i = 0; i < endPoints.size(); i++) {
		auto result = true;
		for (std::size_t j = 1; j < vertices.size(); j++)
			result = result && ((vertices[j] - vertices[j - 1]).Cross(endPoints[i] - vertices[j - 1]) <= 0.e0);
		if (result)
			return true;
	}

	// 線分と矩形の辺とが交差する場合。
	for (std::size_t i = 1; i < vertices.size(); i++) {
		auto diff = vertices[i] - vertices[i - 1];
		auto segment = LineSegmentCollider({ 0.e0, 0.e0 }, std::atan2(diff.y, diff.x), diff.Magnitude());
		if (CheckFor(relativePosition + (vertices[i] + vertices[i - 1]) * 0.5e0, segment))
			return true;
	}
	return false;
}

bool RectangleCollider::CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const
{
	// 矩形が各座標軸と平行になるように回転させる。
	auto circPos = relativePosition.Rotate(-angle);
	auto size = GetSize();
	if (circPos.x > -size.x * 0.5 && circPos.x < size.x * 0.5
			&& circPos.y > -size.y * 0.5 - circle.GetRadius() && circPos.y < size.y * 0.5 + circle.GetRadius())
		return true;
	else if (circPos.x > -size.x * 0.5 - circle.GetRadius() && circPos.x < size.x * 0.5 + circle.GetRadius()
			&& circPos.y > -size.y * 0.5 && circPos.y < size.y * 0.5)
		return true;
	else if (vertices[0].SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0))
		return true;
	else if (vertices[1].SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0))
		return true;
	else if (vertices[2].SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0))
		return true;
	else if (vertices[3].SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0))
		return true;
	else
		return false;
}

// HACK: 何れの矩形の内部にも相手の頂点が含まれない場合は判定できているのか？
bool RectangleCollider::CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const
{
	for (std::size_t i = 0; i < rectangle.vertices.size() - 1; i++) {
		auto result = true;
		for (std::size_t j = 1; j < this->vertices.size(); j++)
			result = result && ((this->vertices[j] - this->vertices[j - 1]).Cross(rectangle.vertices[i] - this->vertices[j - 1]) <= 0.e0);
		if (result)
			return true;
	}
	return false;
}