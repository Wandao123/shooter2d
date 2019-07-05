# Shooter2d（仮称）

マルチプラットフォームな弾幕STG用ゲームエンジンです。エンジン自体はC++で開発しており、ゲームロジック（敵や弾の動き）はLuaで記述します。

## 必要要件

### フォント

WindowsではArialフォント、LinuxではLiberation Sansフォントを使用しています。それぞれ以下のパスにあることが前提です。

- Windows: C:\Windows\Fonts\arial.ttf
- Linux: /usr/share/fonts/TTF/LiberationSans-Regular.ttf

Linux ではディストリビューションに応じてパスが変わりますので、適宜 user_interface.cpp の Filename 定数を修正してください。

### 素材

本プログラムは『東方弾幕風』の素材を使用しております。そのリンクを以下に示します。このような素材をご公開されている皆様にはこの場を借りて感謝申し上げます。素材の関係上、現在のところ本プログラムは東方Projectの二次創作ガイドラインに則ります。（今後自作の素材で置き換えるかもしれません）。

- [自機・自弾・敵](http://coolier.dip.jp/th_up4/index.php?id=6360)
    - Enemy.png
    - Marisadot.png
    - Reimudot.png
    - Sanaedot.png
    - Shot1.png
    - Shot2.png
    - Shot3.png
- [敵弾](http://coolier.dip.jp/th_up3/file/th3_4065.lzh) （えむ様）
    - shot_all.png
- [エフェクト](http://coolier.dip.jp/th_up3/file/th3_7474.lzh) （貴方が作る東方STG～東方弾幕風 8の880 様）
    - effect_circle.png
- 参考：https://danmakufu.wiki.fc2.com/wiki/%E7%B4%A0%E6%9D%90%E3%83%AA%E3%83%B3%E3%82%AF

### 依存ライブラリ

- [SDL 2.0](https://www.libsdl.org/)
- [SDL_image 2.0](https://www.libsdl.org/projects/SDL_image/)
- [SDL_ttf 2.0](https://www.libsdl.org/projects/SDL_ttf/)
- [SDL_mixer 2.0](https://www.libsdl.org/projects/SDL_mixer/)
- [Lua 5.3](https://www.lua.org/) distributed with the [MIT license](https://www.lua.org/license.html)
- [sol2 v2.20.6](https://github.com/ThePhD/sol2) distributed with the [MIT license](https://github.com/ThePhD/sol2/blob/develop/LICENSE.txt)

### コンパイラ

- GCC (>=5)
- Visual C++ (>=15)/Visual Studio (>=2017)

## ビルド方法（暫定）

### Windows (Visual Studio 2019で確認済み)

1. ソースをダウンロードして shooter2d.sln をVisual Studioで開く。
1. NuGetを用いてSDLとSDLのライブラリとLuaをインストールする。ここで「プロジェクト」->「Nuget パッケージの管理」->「復元」を行えば、必要なライブラリが全てダウンロードできる。「復元」ボタンが表示されない場合は「ツール」->「オプション」->「NuGetパッケージマネージャー」からパッケージの復元を有効化する。
1. sol2はNuGetからインストールできないので、リンク先の releases から sol.hpp をダウンロードして shooter2d/packages/sol2 に配置する。
1. 素材をダウンロードして、shooter2d/shooter2d/images 直下に配置する（下記の図を参考にされたい）。
1. Visual StudioからDebugビルドする。「lua.dllが無い」と怒られる場合は shooter2d/packages/lua.regist.*/build/native/bin/v141/x64/Debug/lua.dll を shooter2d/x64/Debug にコピーする。Releaseビルドの場合はReleaseディレクトリに対応するDLLファイルをコピーする。

### Linux OS

1. ソースをダウンロードする。
1. パッケージマネージャーなどを用いてSDLとSDLのライブラリとLuaをインストールする。例えば、Debian系ならAPTからインストールできる：
    ```bash
    $ apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev liblua5.3-dev
    ````
1. sol2は別途ダウンロードして、shooter2d/packages/sol2/sol.hpp に配置する。
1. 素材のファイルをダウンロードして、shooter2d/shooter2d/images 直下に配置する（下記の図を参考にされたい）。
1. shooter2d/shooter2d/lib ディレクトリを作成する。
1. shooter2d/shooter2d に移動して、`make debug` または `make release` を実行する。
1. 実行時にフォントが見つからないというエラーが出る場合は、user_interface.cpp を修正する。

### ディレクトリ構成 (2019/07/05)

~~~
shooter2d
├── packages
│   ├── lua.5.3.*
│   ├── lua.redist.5.3.*
│   ├── sdl2.nuget.2.0.*
│   ├── sdl2.nuget.redist.2.0.*
│   ├── sdl2_image.nuget.2.0.*
│   ├── sdl2_image.nuget.redist.2.0.*
│   ├── sdl2_mixer.nuget.2.0.*
│   ├── sdl2_mixer.nuget.redist.2.0.*
│   ├── sdl2_ttf.nuget.2.0.*
│   ├── sdl2_ttf.nuget.redist.2.0.*
│   └── sol2
│       └── sol.hpp
├── shooter2d
│   ├── Makefile
│   ├── images
│   │   ├── Enemy.png
│   │   ├── Marisadot.png
│   │   ├── Reimudot.png
│   │   ├── Sanaedot.png
│   │   ├── Shot1.png
│   │   ├── Shot2.png
│   │   ├── Shot3.png
│   │   ├── effect_circle.png
│   │   └── shot_all.png
│   ├── lib
│   ├── scripts
│   │   └── stage.lua
│   ├── shooter2d.vcxproj
│   ├── *.cpp
│   └── *.h
├── shooter2d.sln
└── x64
    ├── Debug
    └── Release
~~~

## ライセンス

This software is distributed under the terms of the zlib license reproduced [here](LICENSE).