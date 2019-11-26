#include "collider.h"
#include <array>

using namespace Shooter;

/******************************** Collider *********************************/

Vector2<float> Collider::GetPosition() const
{
	return position;
}

void Collider::SetPosition(const Vector2<float>& position)
{
	this->position = position;
}

float Collider::GetAngle() const
{
	return angle;
}

void Collider::SetAngle(const float angle)
{
	this->angle = MathUtils::Modulo(angle, 2 * M_PI);  // 負の値が渡されても、0 <= angle < 2 pi になるように変換する。
}

/******************************** CircleCollider *********************************/

/// <summary>受け取ったColliderクラスのCheckForを呼び出す。</summary>
/// <param name="relativeVector">保有先のMover同士の相対ベクトル</param>
/// <param name="collider">衝突判定する対象</param>
/// <remarks>relativeVectorはcolliderの保有先Mover（相手）を基準点とする。</remarks>
bool CircleCollider::DoesCollideWith(const Vector2<float>& relativeVector, Collider& collider) const
{
	return collider.CheckFor(position + relativeVector - collider.GetPosition(), *this);
}

/// <summary>CircleColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativeVector">Collider同士の相対ベクトル</param>
/// <param name="circle">衝突判定する対象</param>
/// <remarks>relativeVectorは自分を基準点とする。</remarks>
bool CircleCollider::CheckFor(const Vector2<float>& relativeVector, const CircleCollider& circle) const
{
	if (relativeVector.SquaredMagnitude() <= std::pow(circle.GetRadius() + radius, 2.0f))
		return true;
	else
		return false;
}

/// <summary>RectangleColliderクラスとの衝突判定を行う。</summary>
/// <param name="relativeVector">Collider同士の相対ベクトル</param>
/// <param name="rectangle">衝突判定する対象</param>
/// <remarks>relativeVectorは自分を基準点とする。</remarks>
bool CircleCollider::CheckFor(const Vector2<float>& relativeVector, const RectangleCollider& rectangle) const
{
	// 矩形の中心を座標系の原点に取る。
	auto circPos = (-relativeVector).Rotate(-rectangle.GetAngle());
	if (circPos.x > -rectangle.GetWidth() * 0.5f && circPos.x < rectangle.GetWidth() * 0.5f
			&& circPos.y > -rectangle.GetHeight() * 0.5f - radius && circPos.y < rectangle.GetHeight() * 0.5f + radius)
		return true;
	else if (circPos.x > -rectangle.GetWidth() * 0.5f - radius && circPos.x < rectangle.GetWidth() * 0.5f + radius
			&& circPos.y > -rectangle.GetHeight() * 0.5f && circPos.y < rectangle.GetHeight() * 0.5f)
		return true;
	else if ((Vector2<float>{ -rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5f).SquaredMagnitude() < std::pow(radius, 2.0f))
		return true;
	else if ((Vector2<float>{ -rectangle.GetWidth(), rectangle.GetHeight() } * 0.5f).SquaredMagnitude() < std::pow(radius, 2.0f))
		return true;
	else if ((Vector2<float>{ rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5f).SquaredMagnitude() < std::pow(radius, 2.0f))
		return true;
	else if ((Vector2<float>{ rectangle.GetWidth(), rectangle.GetHeight() } * 0.5f).SquaredMagnitude() < std::pow(radius, 2.0f))
		return true;
	else
		return false;
}

float CircleCollider::GetRadius() const
{
	return radius;
}

/******************************** RectangleCollider *********************************/

/// <summary>受け取ったColliderクラスのCheckForを呼び出す。</summary>
/// <param name="relativeVector">保有先のMover同士の相対ベクトル</param>
/// <param name="collider">衝突判定する対象</param>
/// <remarks>relativeVectorはcolliderの保有先Mover（相手）を基準点とする。</remarks>
bool RectangleCollider::DoesCollideWith(const Vector2<float>& relativeVector, Collider& collider) const
{
	return collider.CheckFor(position + relativeVector - collider.GetPosition(), *this);
}

bool RectangleCollider::CheckFor(const Vector2<float>& relativeVector, const CircleCollider& circle) const
{
	// 矩形の中心を座標系の原点に取る。
	auto circPos = relativeVector.Rotate(-angle);
	if (circPos.x > -width * 0.5f && circPos.x < width * 0.5f
			&& circPos.y > -height * 0.5f - circle.GetRadius() && circPos.y < height * 0.5f + circle.GetRadius())
		return true;
	else if (circPos.x > -width * 0.5f - circle.GetRadius() && circPos.x < width * 0.5f + circle.GetRadius()
			&& circPos.y > -height * 0.5f && circPos.y < height * 0.5f)
		return true;
	else if ((Vector2<float>{ -width, -height } * 0.5f).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0f))
		return true;
	else if ((Vector2<float>{ -width, height } * 0.5f).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0f))
		return true;
	else if ((Vector2<float>{ width, -height } * 0.5f).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0f))
		return true;
	else if ((Vector2<float>{ width, height } * 0.5f).SquaredMagnitude() < std::pow(circle.GetRadius(), 2.0f))
		return true;
	else
		return false;
}

bool RectangleCollider::CheckFor(const Vector2<float>& relativeVector, const RectangleCollider& rectangle) const
{
	std::array<Vector2<float>, 5> rect1 = {  // 自分の四隅の座標。隣同士の差を取ると四辺のベクトルになるようにする。
		(Vector2<float>{ -width, -height } * 0.5f).Rotate(angle),
		(Vector2<float>{ -width, height } * 0.5f).Rotate(angle),
		(Vector2<float>{ width, height } * 0.5f).Rotate(angle),
		(Vector2<float>{ width, -height } * 0.5f).Rotate(angle),
		(Vector2<float>{ -width, -height } * 0.5f).Rotate(angle)
	};
	std::array<Vector2<float>, 4> rect2 = {  // 相手の四隅の座標。
		relativeVector + (Vector2<float>{ -rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5f).Rotate(rectangle.GetAngle()),
		relativeVector + (Vector2<float>{ -rectangle.GetWidth(), rectangle.GetHeight() } * 0.5f).Rotate(rectangle.GetAngle()),
		relativeVector + (Vector2<float>{ rectangle.GetWidth(), -rectangle.GetHeight() } * 0.5f).Rotate(rectangle.GetAngle()),
		relativeVector + (Vector2<float>{ rectangle.GetWidth(), rectangle.GetHeight() } * 0.5f).Rotate(rectangle.GetAngle())
	};
	for (std::size_t i = 1; i < rect1.size(); i++)
		for (std::size_t j = 0; j < rect1.size(); j++)
			if ((rect1[i] - rect1[i - 1]).Cross(rect2[j] - rect1[i - 1]) <= 0.0f)
				return true;
	return false;
}

float RectangleCollider::GetHeight() const
{
	return height;
}

float RectangleCollider::GetWidth() const
{
	return width;
}
