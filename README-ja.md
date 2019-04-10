# Shooter2d（仮称）

マルチプラットフォームな弾幕STG用ゲームエンジンです。エンジン自体はC++で開発しており、ゲームロジック（敵や弾の動き）はLuaで記述します。

## 必要要件

### フォント

WindowsではArialフォント、LinuxではLiberation Sansフォントを使用（後々、画像で差し替え予定）。

### 素材

（後々、自作のものに差し替え予定）

- 自機・敵 (http://coolier.dip.jp/th_up4/index.php?id=6360) --> shooter2d/images/に配置。

### 依存ライブラリ

- SDL 2.0 (https://www.libsdl.org/)
- SDL_image 2.0 (https://www.libsdl.org/projects/SDL_image/)
- SDL_ttf 2.0 (https://www.libsdl.org/projects/SDL_ttf/)
- Lua 5.3 (https://www.lua.org/)
- sol2 (https://github.com/ThePhD/sol2) --> packages/sol/に配置。

## ライセンス

T.B.D.（現在選定中）