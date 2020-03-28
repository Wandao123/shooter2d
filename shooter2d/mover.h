#ifndef MOVER_H
#define MOVER_H

#include "assets.h"
#include "collider.h"
#include "game_object.h"
#include <functional>

namespace Shooter {
	/// <summary>衝突判定の対象となるオブジェクト。自機と敵と弾の親クラス。</summary>
	/// <remarks>
	/// デフォルトでenabledにfalseを設定するため、生成しただけでは更新されない。継承先で「実体化関数」を定義する必要がある。分けたのは、各クラスで渡すべき引数が異なるため。
	/// </remarks>
	class Mover : public GameObject
	{
	public:
		Mover(const Vector2<double>& position, const double speed, const double angle, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, const unsigned int damage, const int hitPoint, bool autoDisabling = true);
		virtual void Draw() const override;
		virtual void Update() override;
		virtual void OnCollide(Mover& mover) = 0;

		double GetSpeed() const
		{
			return speed;
		}

		void SetSpeed(const double speed)
		{
			if (enabled)
				this->speed = speed;
		}

		double GetAngle() const
		{
			return angle;
		}

		void SetAngle(const double angle)
		{
			if (enabled)
				this->angle = MathUtils::Modulo(angle, 2 * M_PI);  // 負の値が渡されても、0 <= angle < 2 pi になるように変換する。
		}

		Collider& GetCollider() const&
		{
			return *collider;
		}

		unsigned int GetDamage() const
		{
			return damage;
		}

		int GetHitPoint() const
		{
			return hitPoint;
		}

		unsigned int IsInvincible() const
		{
			return (invincibleCounter > 0) ? true : false;
		}

		void TurnInvincible(const unsigned int frames)
		{
			invincibleCounter = frames;
		}
	protected:
		double speed;  // 単位：ドット毎フレーム
		double angle;  // x軸を基準とした角度。時計回りの方向を正とする。
		unsigned int damage;  // 衝突時に相手に与えるダメージ。
		int hitPoint;  // 体力。「消滅」と「撃破」とを区別するために弾でも設定が必須。外部からこれを参照する以外に、OnCollideに返り値を持たせる実装でも良いかもしれない。
		std::unique_ptr<Sprite> sprite;
		std::unique_ptr<Collider> collider;
		unsigned int invincibleCounter;  // 無敵状態になっている残りのフレーム数。
		virtual void spawned();

		/// <summary>現在のフレームにおける画像の切り取り位置を返す。</summary>
		/// <param name="currentFrames">現在までのフレーム数</param>
		/// <returns>切り取る矩形</returns>
		virtual Rect<int>& clipFromImage(unsigned int countedFrames) = 0;
	private:
		unsigned int existingCounter;  // enabledがtrueになってからのフレーム数。
		std::function<void(void)> disableIfOutside;
		bool isInside();
	};
}

#endif // !MOVER_H
