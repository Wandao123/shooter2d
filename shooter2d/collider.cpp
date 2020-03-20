#include "collider.h"
#include <array>

using namespace Shooter;

/******************************** Collider *********************************/

const Vector2<double>& Collider::GetOffset() const&
{
	return offset;
}

void Collider::SetOffset(const Vector2<double>& offset)
{
	this->offset = offset;
}

double Collider::GetAngle() const
{
	return angle;
}

void Collider::SetAngle(const double angle)
{
	this->angle = MathUtils::Modulo(angle, 2 * M_PI);  // 負の値が渡されても、0 <= angle < 2 pi になるように変換する。
}

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
	auto direction1 = MathUtils::PolarCoordinateToVector<double>(length, angle);
	auto direction2 = relativePosition + MathUtils::PolarCoordinateToVector<double>(lineSegment.GetLength(), lineSegment.GetAngle());
	auto differenceBetweenInitialPoints = direction1 * 0.5e0 + relativePosition - direction2 * 0.5e0;
	if (direction1.Cross(direction2) < EPS)
		return false;
	if (differenceBetweenInitialPoints.Cross(direction2) / direction1.Cross(direction2) >= 0.e0
			&& differenceBetweenInitialPoints.Cross(direction2) / direction1.Cross(direction2) <= 1.e0
			&& differenceBetweenInitialPoints.Cross(direction1) / direction1.Cross(direction2) >= 0.e0
			&& differenceBetweenInitialPoints.Cross(direction1) / direction1.Cross(direction2) <= 1.e0)
		return true;
	return false;
}

/// <summary>CircleColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="circle">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool LineSegmentCollider::CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const
{
	auto direction = MathUtils::PolarCoordinateToVector<double>(length, angle);
	if (std::abs(direction.Normalize().Cross(relativePosition)) > circle.GetRadius())
		return false;
	if ((relativePosition + direction * 0.5e0).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.e0)
			|| (relativePosition - direction * 0.5e0).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.e0))
		return true;
	if ((relativePosition + direction * 0.5e0).Dot(direction) * (relativePosition + direction * 0.5e0).Dot(direction) <= 0.e0)
		return true;
	return false;
}

/// <summary>RectangleColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativePosition">Collider同士の相対位置ベクトル</param>
/// <param name="rectangle">衝突判定する対象</param>
/// <remarks>relativePositionは自分を基準点とする。</remarks>
bool LineSegmentCollider::CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const
{
	auto direction = MathUtils::PolarCoordinateToVector<double>(length, angle);
	std::array<Vector2<double>, 2> endPoints = { relativePosition + direction * 0.5e0, relativePosition - direction * 0.5e0 };
	std::array<Vector2<double>, 5> rect = {  // 相手の四隅の座標。隣同士の差を取ると四辺のベクトルになるようにする。
		relativePosition + (Vector2<double>{ -rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle()),
		relativePosition + (Vector2<double>{ -rectangle.GetWidth(), rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle()),
		relativePosition + (Vector2<double>{ rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle()),
		relativePosition + (Vector2<double>{ rectangle.GetWidth(), rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle()),
		relativePosition + (Vector2<double>{ -rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle())
	};

	// 線分の端点が矩形に含まれる場合。
	for (std::size_t i = 0; i < endPoints.size(); i++) {
		auto result = true;
		for (std::size_t j = 1; j < rect.size(); j++)
			result = result && ((rect[j] - rect[j - 1]).Cross(endPoints[i] - rect[j - 1]) <= 0.e0);
		if (result)
			return true;
	}

	// 線分と矩形の辺とが交差する場合。
	for (std::size_t i = 1; i < rect.size(); i++) {
		auto diff = rect[i] - rect[i - 1];
		auto segment = LineSegmentCollider({ 0.e0, 0.e0 }, std::atan2(diff.y, diff.x), diff.Magnitude());
		if (CheckFor(relativePosition + (rect[i] + rect[i - 1]) * 0.5e0, segment))
			return true;
	}
	return false;
}

/*/// <summary>方向ベクトルを取得する。</summary>
/// <returns>方向ベクトルへの参照</returns>
const Vector2<double>& LineSegmentCollider::GetOrientation() const&
{
	return orientation;
}


/// <summary>終点を取得する。</summary>
/// <returns>終点のベクトル</returns>
/// <remarks>始点を取得するにはGetOffsetを使う。</remarks>
Vector2<double> LineSegmentCollider::GetTerminalPoint() const
{
	return offset + orientation;
}*/

double LineSegmentCollider::GetLength() const
{
	return length;
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
	// 矩形の中心を座標系の原点に取る。
	auto circPos = (-relativePosition).Rotate(-rectangle.GetAngle());
	if (circPos.x > -rectangle.GetWidth() * 0.5 && circPos.x < rectangle.GetWidth() * 0.5
			&& circPos.y > -rectangle.GetHeight() * 0.5 - radius && circPos.y < rectangle.GetHeight() * 0.5 + radius)
		return true;
	else if (circPos.x > -rectangle.GetWidth() * 0.5 - radius && circPos.x < rectangle.GetWidth() * 0.5 + radius
			&& circPos.y > -rectangle.GetHeight() * 0.5 && circPos.y < rectangle.GetHeight() * 0.5)
		return true;
	else if ((Vector2<double>{ -rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5).SquaredMagnitude() < std::pow(radius, 2.0))
		return true;
	else if ((Vector2<double>{ -rectangle.GetWidth(), rectangle.GetHeight() } * 0.5).SquaredMagnitude() < std::pow(radius, 2.0))
		return true;
	else if ((Vector2<double>{ rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5).SquaredMagnitude() < std::pow(radius, 2.0))
		return true;
	else if ((Vector2<double>{ rectangle.GetWidth(), rectangle.GetHeight() } * 0.5).SquaredMagnitude() < std::pow(radius, 2.0))
		return true;
	else
		return false;
}

double CircleCollider::GetRadius() const
{
	return radius;
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
	std::array<Vector2<double>, 5> rect = {  // 自分の四隅の座標。隣同士の差を取ると四辺のベクトルになるようにする。
		(Vector2<double>{ -width, -height } * 0.5).Rotate(angle),
		(Vector2<double>{ -width, height } * 0.5).Rotate(angle),
		(Vector2<double>{ width, height } * 0.5).Rotate(angle),
		(Vector2<double>{ width, -height } * 0.5).Rotate(angle),
		(Vector2<double>{ -width, -height } * 0.5).Rotate(angle)
	};
	auto direction = MathUtils::PolarCoordinateToVector<double>(lineSegment.GetLength(), lineSegment.GetAngle());
	std::array<Vector2<double>, 2> endPoints = { relativePosition + direction * 0.5e0, relativePosition - direction * 0.5e0 };

	// 線分の端点が矩形に含まれる場合。
	for (std::size_t i = 0; i < endPoints.size(); i++) {
		auto result = true;
		for (std::size_t j = 1; j < rect.size(); j++)
			result = result && ((rect[j] - rect[j - 1]).Cross(endPoints[i] - rect[j - 1]) <= 0.e0);
		if (result)
			return true;
	}

	// 線分と矩形の辺とが交差する場合。
	for (std::size_t i = 1; i < rect.size(); i++) {
		auto diff = rect[i] - rect[i - 1];
		auto segment = LineSegmentCollider({ 0.e0, 0.e0 }, std::atan2(diff.y, diff.x), diff.Magnitude());
		if (CheckFor(relativePosition + (rect[i] + rect[i - 1]) * 0.5e0, segment))
			return true;
	}
	return false;
}

bool RectangleCollider::CheckFor(const Vector2<double>& relativePosition, const CircleCollider& circle) const
{
	// 矩形の中心を座標系の原点に取る。
	auto circPos = relativePosition.Rotate(-angle);
	if (circPos.x > -width * 0.5 && circPos.x < width * 0.5
			&& circPos.y > -height * 0.5 - circle.GetRadius() && circPos.y < height * 0.5 + circle.GetRadius())
		return true;
	else if (circPos.x > -width * 0.5 - circle.GetRadius() && circPos.x < width * 0.5 + circle.GetRadius()
			&& circPos.y > -height * 0.5 && circPos.y < height * 0.5)
		return true;
	else if ((Vector2<double>{ -width, -height } * 0.5).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0))
		return true;
	else if ((Vector2<double>{ -width, height } * 0.5).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0))
		return true;
	else if ((Vector2<double>{ width, -height } * 0.5).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0))
		return true;
	else if ((Vector2<double>{ width, height } * 0.5).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0))
		return true;
	else
		return false;
}

// HACK: 何れの矩形の内部にも相手の頂点が含まれない場合は判定できているのか？
bool RectangleCollider::CheckFor(const Vector2<double>& relativePosition, const RectangleCollider& rectangle) const
{
	std::array<Vector2<double>, 5> rect1 = {  // 自分の四隅の座標。隣同士の差を取ると四辺のベクトルになるようにする。
		(Vector2<double>{ -width, -height } * 0.5).Rotate(angle),
		(Vector2<double>{ -width, height } * 0.5).Rotate(angle),
		(Vector2<double>{ width, height } * 0.5).Rotate(angle),
		(Vector2<double>{ width, -height } * 0.5).Rotate(angle),
		(Vector2<double>{ -width, -height } * 0.5).Rotate(angle)
	};
	std::array<Vector2<double>, 4> rect2 = {  // 相手の四隅の座標。
		relativePosition + (Vector2<double>{ -rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle()),
		relativePosition + (Vector2<double>{ -rectangle.GetWidth(), rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle()),
		relativePosition + (Vector2<double>{ rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle()),
		relativePosition + (Vector2<double>{ rectangle.GetWidth(), rectangle.GetHeight() } * 0.5).Rotate(rectangle.GetAngle())
	};
	for (std::size_t i = 0; i < rect2.size(); i++) {
		auto result = true;
		for (std::size_t j = 1; j < rect1.size(); j++)
			result = result && ((rect1[j] - rect1[j - 1]).Cross(rect2[i] - rect1[j - 1]) <= 0.e0);
		if (result)
			return true;
	}
	return false;
}

double RectangleCollider::GetHeight() const
{
	return height;
}

double RectangleCollider::GetWidth() const
{
	return width;
}
