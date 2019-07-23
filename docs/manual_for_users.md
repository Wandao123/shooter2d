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
</dl>

### 列挙型定数

それぞれ \*ID.*name* の形でアクセスします。列挙型の定数として定義されています。

#### 敵 (EnemyID)

- SmallRed
- SmallBlue

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

#### 画面 (SceneID)

- Title（タイトル画面）
- StageClear（ステージ・クリア画面）
- AllClear（全ステージ・クリア画面）

### 生成関数

<dl>
    <dt>Bullet GenerateBullet(BulletID id, float posX, float posY, float speed, float angle) [overloaded]</dt>
    <dd>弾オブジェクトを初期状態に従って生成します。</dd>
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
    <dt>Bullet GenerateBullet(BulletID id, Enemy enemy, float speed, float angle) [overloaded]</dt>
    <dd>渡された敵オブジェクトの位置に、弾オブジェクトを初期状態に従って生成します。敵オブジェクトが有効でない場合はnilを返します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>生成されたオブジェクト</li>
            <li><b>id: </b>生成する弾のID</li>
            <li><b>enemy: </b>生成元の敵オブジェクト</li>
            <li><b>speed: </b>初期速度の大きさ（速さ）</li>
            <li><b>angle: </b>初期速度の角度（向き）</li>
        </ol>
    </dd>
    <dt>Enemy GenerateEnemy(EnemyID id, float posX, float posY, float speed, float angle, int hitPoint)</dt>
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
</dl>

### 自機・敵・弾で共通の関数

<dl>
    <dt>float <i>Object</i>.Angle property</dt>
    <dd>オブジェクトのx軸に対する回転角を表します。自機オブジェクトでは読み込み専用です。</dd>
    <dt>float <i>Object</i>.PosX property (read only)</dt>
    <dd>オブジェクトの位置のx座標を取得します。</dd>
    <dt>float <i>Object</i>.PosY property (read only)</dt>
    <dd>オブジェクトの位置のy座標を取得します。</dd>
    <dt>float <i>Object</i>.Speed property</dt>
    <dd>オブジェクトの速さを表します。単位は1ドット毎フレームです。自機オブジェクトでは読み込み専用です。</dd>
    <dt>bool <i>Object</i>.IsEnabled(void)</dt>
    <dd>オブジェクトが有効ならばtrue、無効ならばfalseを返します。</dd>
</dl>

### 敵・弾で共通の関数

<dl>
    <dt>void <i>Object</i>.Erase(void)</dt>
    <dd>オブジェクトを消去（無効に）します。このとき、消滅エフェクトは実行されません。</dd>
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
    <dt>Player GetPlayer(void)</dt>
    <dd>自機オブジェクトを取得します。</dd>
    <dd>
        <ol>
            <li><b>返り値: </b>自機オブジェクト</li>
        </ol>
    </dd>
    <dt>void StartCoroutine(func) [overloaded]</dt>
    <dd>毎フレーム実行するコルーチンfuncを登録します。Luaのresume関数と異なり、1フレーム毎に自動的に実行されます。この関数を呼び出した時点でfuncは1回実行されます。funcには引数を渡しません。</dd>
    <dd>
        <ol>
            <li><b>func: </b>Luaの関数</li>
        </ol>
    </dd>
    <dt>void StartCoroutine(func, arg1, arg2, ...) [overloaded]</dt>
    <dd>毎フレーム実行するコルーチンfuncを登録します。Luaのresume関数と異なり、1フレーム毎に自動的に実行されます。この関数を呼び出した時点でfuncは1回実行されます。funcには arg1, arg2, ... の形式で引数を渡します。</dd>
    <dd>
        <ol>
            <li><b>func: </b>Luaの関数</li>
            <li><b>(arg1, arg2, ...): </b>引数リスト</li>
        </ol>
    </dd>
</dl>