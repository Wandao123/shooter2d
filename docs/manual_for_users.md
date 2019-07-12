# 一般向け説明書

ここではLuaでの記述のために用意した関数群や、オブジェクト（自機、敵、弾、エフェクト）の追加方法を解説します。

## 基本的な書き方

エンジンは最初にmain.luaファイルのMain関数を読み込みます。

座標系：画面右向きをx軸の正、画面下向きをy軸の正とします。それに伴って、x軸を基準とした時計回りの方向を角度の正の向きと定めます。

## スクリプト・リファレンス

### 定数

#### int ScreenHeight

画面の縦幅を取得します。

#### int ScreenWidth

画面の横幅を取得します。

### オブジェクトID

それぞれ *object*ID.*name* の形でアクセスします。

#### 弾 (BulletID)

- 大弾
    - LargeRed
    - LargeBlue
- 中弾
    - MiddleRed
    - MiddleBlue
- 小弾
    - SmallRed
    - SmallBlue
- 粒弾
    - TinyRed
    - TinyBlue
- 鱗弾
    - ScaleRed
    - ScaleBlue

#### 敵 (EnemyID)

- SmallRed
- SmallBlue

### 生成関数

#### Bullet GenerateBullet(BulletID id, float posX, float posY, float speed, float angle) _overloaded_

弾オブジェクトを生成した上で、初期状態に従って表示します。

1. __id:__ 生成する弾のID
1. __posX:__ 初期位置のx座標
1. __posY:__ 初期位置のy座標
1. __speed:__ 初期速度の大きさ（速さ）
1. __angle:__ 初期速度の角度（向き）

#### Bullet GenerateBullet(BulletID id, Enemy enemy, float speed, float angle) _overloaded_

弾オブジェクトを生成した上で、初期状態に従って表示します。

1. __id:__ 生成する弾のID
1. __enemy:__ 生成元の敵オブジェクト
1. __speed:__ 初期速度の大きさ（速さ）
1. __angle:__ 初期速度の角度（向き）

#### Enemy GenerateEnemy(EnemyID id, float posX, float posY, float speed, float angle, int hitPoint)

敵オブジェクトを生成した上で、初期状態に従って表示します。

1. __id:__ 生成する敵のID
1. __posX:__ 初期位置のx座標
1. __posY:__ 初期位置のy座標
1. __speed:__ 初期速度の大きさ（速さ）
1. __angle:__ 初期速度の角度（向き）
1. __hitPoint:__ 敵の体力

### 自機・敵・弾で共通の関数

#### float Object.Angle _property_

オブジェクトのx軸に対する回転角を表します。自機オブジェクトでは読み込み専用です。

#### float Object.PosX _property (read only)_

オブジェクトの位置のx座標を取得します。

#### float Object.PosY _property (read only)_

オブジェクトの位置のy座標を取得します。

#### float Object.Speed _property_

オブジェクトの速さを表します。単位は1ドット毎フレームです。自機オブジェクトでは読み込み専用です。

### その他

#### Player GetPlayer(void)

自機オブジェクトを取得します。

#### void StartCoroutine(func) _overloaded_

funcコルーチンを1フレーム毎に `coroutine.yield` が呼ばれるまで実行します。funcには引数を渡しません。

1. __func:__ Luaの関数

#### void StartCoroutine(func, arg1, arg2, ...) _overloaded_

funcコルーチンを1フレーム毎に `coroutine.yield` が呼ばれるまで実行します。funcには (arg1, arg2, ...) という引数を渡します。

1. __func:__ Luaの関数
1. __(arg1, arg2, ...):__ 引数リスト