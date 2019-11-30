# 一般向け説明書

ここではLuaでの記述のために用意した関数群や、オブジェクト（自機、敵、弾、エフェクト）の追加方法を解説します。

## 基本的な書き方

エンジンは最初にmain.luaファイルのMain関数を読み込みます。

全てのコルーチンとMain関数が終了していればタイトル画面に遷移します。

オブジェクトが有効であるとき、オブジェクトの状態の更新と描画を行います。

座標系：画面右向きをx軸の正、画面下向きをy軸の正とします。それに伴って、x軸を基準とした時計回りの方向を角度の正の向きと定めます。

## スクリプト・リファレンス

### 定数

<dl>
    <dt>int ScreenHeight</dt>
    <dd>画面の縦幅を表します。</dd>
    <dt>int ScreenWidth</dt>
    <dd>画面の横幅を表します。</dd>
    <dt>int PlayerHeight</dt>
    <dd>自機のスプライトの縦幅を表します。</dd>
    <dt>int PlayerWidth</dt>
    <dd>自機のスプライトの横幅を表します。</dd>
</dl>

### 列挙型定数

それぞれ \*ID.*name* の形でアクセスします。列挙型の定数として定義されています。

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
- 米弾
    - RicdRed
    - RicdBlue
- 自機のショット
    - ReimuNormal
    - MarisaNormal
    - SanaeNormal

#### 入力コマンド (CommandID)

- Shot
- Bomb
- Slow
- Skip
- Leftward
- Rightward
- Forward
- Backward
- Pause

#### 敵 (EnemyID)

- SmallRed
- SmallBlue

#### 自機 (PlayerID)

- Reimu
- Marisa
- Sanae

#### 画面 (SceneID)

- Title（タイトル画面）
- StageClear（ステージ・クリア画面）
- AllClear（全ステージ・クリア画面）

### 生成関数

<dl>
    <dt>Bullet GenerateBullet(BulletID id, double posX, double posY, double speed, double angle) [overloaded]</dt>
    <dd>敵弾オブジェクトを初期状態に従って生成します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>生成されたオブジェクト</li>
            <li><b>id: </b>生成する弾のID</li>
            <li><b>posX: </b>初期位置のx座標</li>
            <li><b>posY: </b>初期位置のy座標</li>
            <li><b>speed: </b>初期速度の大きさ（速さ）</li>
            <li><b>angle: </b>初期速度の角度（向き）</li>
        </ol>
    </dd>
    <dt>Bullet GenerateBullet(BulletID id, Enemy enemy, double speed, double angle) [overloaded]</dt>
    <dd>渡された敵オブジェクトの位置に、敵弾オブジェクトを初期状態に従って生成します。敵オブジェクトが有効でない場合はnilを返します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>生成されたオブジェクト</li>
            <li><b>id: </b>生成する弾のID</li>
            <li><b>enemy: </b>生成元の敵オブジェクト</li>
            <li><b>speed: </b>初期速度の大きさ（速さ）</li>
            <li><b>angle: </b>初期速度の角度（向き）</li>
        </ol>
    </dd>
    <dt>Enemy GenerateEnemy(EnemyID id, double posX, double posY, double speed, double angle, int hitPoint)</dt>
    <dd>敵オブジェクトを初期状態に従って生成します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>生成されたオブジェクト</li>
            <li><b>id: </b>生成する敵のID</li>
            <li><b>posX: </b>初期位置のx座標</li>
            <li><b>posY: </b>初期位置のy座標</li>
            <li><b>speed: </b>初期速度の大きさ（速さ）</li>
            <li><b>angle: </b>初期速度の角度（向き）</li>
            <li><b>hitPoint: </b>敵の体力</li>
        </ol>
    </dd>
    <dt>Player GeneratePlayer(PlayerID id, double posX, double posY)</dt>
    <dd>自機オブジェクトを初期状態に従って生成します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>生成されたオブジェクト</li>
            <li><b>id: </b>生成する自機のID</li>
            <li><b>posX: </b>初期位置のx座標</li>
            <li><b>posY: </b>初期位置のy座標</li>
        </ol>
    </dd>
    <dt>Bullet GeneratePlayerBullet(BulletID id, double posX, double posY, double speed, double angle)</dt>
    <dd>自弾オブジェクトを初期状態に従って生成します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>生成されたオブジェクト</li>
            <li><b>id: </b>生成する弾のID</li>
            <li><b>posX: </b>初期位置のx座標</li>
            <li><b>posY: </b>初期位置のy座標</li>
            <li><b>speed: </b>初期速度の大きさ（速さ）</li>
            <li><b>angle: </b>初期速度の角度（向き）</li>
        </ol>
    </dd>
</dl>

### 弾・敵・自機で共通の関数

<dl>
    <dt>double <i>Object</i>.Angle property</dt>
    <dd>オブジェクトのx軸に対する回転角を表します。自機オブジェクトでは読み込み専用です。</dd>
    <dt>double <i>Object</i>.PosX property</dt>
    <dd>オブジェクトの位置のx座標を取得します。</dd>
    <dt>double <i>Object</i>.PosY property</dt>
    <dd>オブジェクトの位置のy座標を取得します。</dd>
    <dt>double <i>Object</i>.Speed property</dt>
    <dd>オブジェクトの速さを表します。単位は1ドット毎フレームです。自機オブジェクトでは読み込み専用です。</dd>
    <dt>bool <i>Object</i>.IsEnabled(void)</dt>
    <dd>オブジェクトが有効ならばtrue、無効ならばfalseを返します。</dd>
    <dt>void <i>Object</i>.TurnInvincible(unsigned int frames)</dt>
    <dd>指定したフレーム数だけ無敵状態になります。</dd>
    <dd>
        <ol>
            <li><b>frames: </b>無敵状態にするフレーム数。</li>
        </ol>
    </dd>
</dl>

### 弾・敵で共通の関数

<dl>
    <dt>void <i>Object</i>.Erase(void)</dt>
    <dd>オブジェクトを消去（無効に）します。このとき、消滅エフェクトは実行されません。</dd>
</dl>

### 自機ブジェクトの関数

<dl>
    <dt>int <i>Object</i>.Life property</dt>
    <dd>自機の残機を表します。</dd>
    <dt>void SetVelocity(double dirX, double, dirY, bool slowMode)</dt>
    <dd>自機の速度を設定します。方向は指定されたもの、速さはあらかじめ自機オブジェクトが持っているものが使われます。</dd>
    <dd>
        <ol>
            <li><b>dirX: </b>速度のx成分</li>
            <li><b>dirY: </b>速度のy成分</li>
            <li><b>slowMode: </b>低速移動モードにするか否か（falseのとき高速、trueのとき低速に速さを設定する）</li>
        </ol>
    </dd>
    <dt>void Spawned(void)</dt>
    <dd>オブジェクトが無効の場合、現在の設定された位置でオブジェクトを有効化します。このとき、<i>Object</i>.Lifeが1減少します。</dd>
</dl>

### 敵オブジェクトの関数

<dl>
    <dt>int <i>Object</i>.HitPoint property (read only)</dt>
    <dd>敵の体力を表します。</dd>
</dl>

### その他

<dl>
    <dt>void ChangeScene(SceneID id)</dt>
    <dd>ゲーム画面からidで指定した画面に遷移します。StageClearを指定した場合、一旦ステージ・クリア画面に遷移した後にゲーム画面に戻り、Main関数の続きからスクリプトを実行します。AllClearを指定した場合、全ステージ・クリア画面に遷移した後にタイトル画面に遷移します。</dd>
    <dd>
        <ol>
            <li><b>id: </b>遷移先の画面のID</li>
        </ol>
    </dd>
    <dt>bool GetKey(CommandID command)</dt>
    <dd>コマンド入力を検知します。commandに対応するキー/ボタンが押し続けられている間、trueを返します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>キー/ボタンが押し続けられているか否か</li>
            <li><b>command: </b>検知するコマンド</li>
        </ol>
    </dd>
    <dt>bool GetKeyDown(CommandID command)</dt>
    <dd>コマンド入力を検知します。commandに対応するキー/ボタンが押された瞬間、trueを返します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>キー/ボタンが押されたか否か</li>
            <li><b>command: </b>検知するコマンド</li>
        </ol>
    </dd>
    <dt>bool GetKeyUp(CommandID command)</dt>
    <dd>コマンド入力を検知します。commandに対応するキー/ボタンが離された瞬間、trueを返します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>キー/ボタンが離されたか否か</li>
            <li><b>command: </b>検知するコマンド</li>
        </ol>
    </dd>
    <dt>Player GetPlayer(void)</dt>
    <dd>自機オブジェクトを取得します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>自機オブジェクト</li>
        </ol>
    </dd>
    <dt>list::iterator StartCoroutine(func) [overloaded]</dt>
    <dd>毎フレーム実行するコルーチンfuncを登録します。Luaのresume関数と異なり、1フレーム毎に自動的に実行されます。この関数を呼び出した時点でfuncは1回実行されます。funcには引数を渡しません。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>登録したタスク（スレッドとコルーチンの組）へのイテレータ</li>
            <li><b>func: </b>Luaの関数</li>
        </ol>
    </dd>
    <dt>list::iterator StartCoroutine(func, arg1, arg2, ...) [overloaded]</dt>
    <dd>毎フレーム実行するコルーチンfuncを登録します。Luaのresume関数と異なり、1フレーム毎に自動的に実行されます。この関数を呼び出した時点でfuncは1回実行されます。funcには arg1, arg2, ... の形式で引数を渡します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>登録したタスク（スレッドとコルーチンの組）へのイテレータ</li>
            <li><b>func: </b>Luaの関数</li>
            <li><b>(arg1, arg2, ...): </b>引数リスト</li>
        </ol>
    </dd>
    <dt>void StopCoroutine(task)</dt>
    <dd>指定したコルーチンを停止します。</dd>
    <dd>
        <ol>
            <li><b>task: </b>停止するタスク（スレッドとコルーチンの組）へのイテレータ</li>
        </ol>
    </dd>
</dl>