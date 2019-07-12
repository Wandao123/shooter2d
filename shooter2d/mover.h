#ifndef MOVER_H
#define MOVER_H

#include "collider.h"
#include "effect.h"

namespace Shooter {
	/// <summary>衝突判定の対象となるオブジェクト。自機と敵と弾の親クラス。</summary>
	/// <remarks>
	/// デフォルトでenabledにfalseを設定するため、生成しただけでは更新されない。継承先で「実体化関数」を定義する必要がある。分けたのは、各クラスで渡すべき引数が異なるため。
	/// </remarks>
	class Mover : public GameObject
	{
	public:
		/// <param name="position">初期位置</param>
		/// <param name="speed">初期速度の速さ</param>
		/// <param name="angle">初期速度の方向</param>
		/// <param name="sprite">Spriteクラスへのポインタ（画像はAssetLoaderから指定）</param>
		/// <param name="collider">当たり判定クラスへのポインタ</param>
		/// <param name="effectID">消滅エフェクトのID</param>
		/// <param name="damage">衝突時に相手に与えるダメージ</param>
		Mover(const Vector2& position, const float speed, const float angle, std::unique_ptr<Sprite>&& sprite, std::unique_ptr<Collider>&& collider, EffectManager::EffectID effectID, const unsigned int damage)
			: GameObject(false, position)
			, speed(speed)
			, angle(angle)
			, damage(damage)
			, sprite(std::move(sprite))
			, collider(std::move(collider))
			, effectID(effectID)
		{}
		virtual ~Mover() {}
		virtual void Draw() const override;
		virtual void Update() override;
		virtual void OnCollide(Mover& mover) = 0;

		float GetSpeed() const
		{
			return speed;
		}

		void SetSpeed(const float speed)
		{
			if (enabled)
				this->speed = speed;
		}

		float GetAngle() const
		{
			return angle;
		}

		void SetAngle(const float angle)
		{
			if (enabled)
				this->angle = MathUtils::Modulo(angle, 2 * M_PI);  // 負の値が渡されても、0 <= angle < 2 pi になるように変換する。
		}

		Collider& GetCollider() const
		{
			return *collider;
		}

		EffectManager::EffectID GetEffectID() const
		{
			return effectID;
		}

		unsigned int GetDamage() const
		{
			return damage;
		}
	protected:
		float speed;  // 単位：ドット毎フレーム
		float angle;  // x軸を基準とした角度。時計回りの方向を正とする。
		unsigned int damage;  // 衝突時に相手に与えるダメージ。
		std::unique_ptr<Sprite> sprite;
		std::unique_ptr<Collider> collider;
		EffectManager::EffectID effectID;  // 消滅エフェクトのID。
		bool isInside();
		virtual void spawned();

		/// <summary>現在のフレームにおける画像の切り取り位置を返す。</summary>
		/// <param name="currentFrames">現在までのフレーム数</param>
		/// <returns>切り取る矩形</returns>
		virtual SDL_Rect& clipFromImage(unsigned int countedFrames) = 0;
	private:
		unsigned int counter = 0;  // enabledがtrueになってからのフレーム数。
	};
}

#endif // !MOVER_H
