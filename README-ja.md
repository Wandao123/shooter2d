# Shooter2d（仮称）

マルチプラットフォームな弾幕STG用ゲームフレームワークとして制作しています。エンジン部分はC++で開発しており、ゲームロジック（敵や弾の動き）はLuaで記述します。

## 必要要件

### フォント

WindowsではArialフォント、LinuxではLiberation Sansフォントを使用しています。それぞれ以下のパスにあることが前提です。

- Windows: C:\Windows\Fonts\arial.ttf
- Linux: /usr/share/fonts/TTF/LiberationSans-Regular.ttf

Linux ではディストリビューションに応じてパスが変わりますので、適宜 user_interface.cpp の Filename 定数を修正してください。

### 素材

本プログラムは『東方弾幕風』の素材を使用しております。そのリンクを以下に示します。このような素材をご公開されている皆様にはこの場を借りて感謝申し上げます。素材の関係上、現在のところ本プログラムは東方Projectの二次創作ガイドラインに則ります。（今後自作の素材で置き換えるかもしれません）。

- [自機・自弾・敵画像](http://coolier.dip.jp/th_up4/index.php?id=6360)
    - Enemy.png
    - Marisadot.png
    - Reimudot.png
    - Sanaedot.png
    - Shot1.png
    - Shot2.png
    - Shot3.png
- [敵弾画像](http://coolier.dip.jp/th_up3/file/th3_4065.lzh) （えむ様）
    - shot_all.png
- [エフェクト画像](http://coolier.dip.jp/th_up3/file/th3_7474.lzh) （貴方が作る東方STG～東方弾幕風8の880様）
    - effect_circle.png
- [敵SE](http://coolier.dip.jp/th_up4/index.php?id=2637) （４１３y様）
    - enemy_damage.wav
    - enemy_vanish[エフェクトＡ].wav --> rename this to enemy_vanish_effect-A.wav
    - shot1.wav
- [被弾音](https://commons.nicovideo.jp/material/nc899) （koshibone様）
    - nc899.wav
- [ショット音](http://osabisi.sakura.ne.jp/m2/tm4/se/se_old_pack00.zip) （Osabisi様）
    - sha04.wav

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
- Visual C++ 2019

## 操作方法

- **移動・選択：** 方向キー
- **ショット・決定：** Zキー
- **低速移動：** Shiftキー

現在の設定では3回被弾したらゲームオーバーになります。また、ゲーム開始時にデータを読み込むため、数秒程度ラグが生じます。

## ビルド方法（暫定）

### Windows

#### Visual Studio 2019 + vcpkg

1. [マニュアル](https://docs.microsoft.com/ja-jp/cpp/build/vcpkg?view=vs-2019)および[GitHubのリポジトリ](https://github.com/microsoft/vcpkg)を参考にvcpkgをインストールした上で、それを用いて依存ライブラリをインストールする。具体的にはPowerShellで次のコマンドを実行する：
    ```
    PS> git clone https://github.com/Microsoft/vcpkg.git
    PS> cd vcpkg
    PS> .\bootstrap-vcpkg.bat
    PS> .\vcpkg integrate install
    PS> .\vcpkg install sdl2:x64-windows sdl2-image:x64-windows sdl2-mixer:x64-windows sdl2-ttf:x64-windows lua:x64-windows sol2:x64-windows
    ```
    途中で ``Please install the English language pack.'' などと表示された場合は、Visual Studio Installerから英語の言語パックをインストールする。
1. ソースをダウンロードして、shooter2d.sln をVisual Studioで開く。
1. shooter2d/shooter2d/images ディレクトリと shooter2d/shooter2d/se ディレクトリを作成する。素材をダウンロードして、images と se それぞれにpngファイルとwavファイルを配置する（下記の図を参考にされたい）。
1. Visual StudioからDebugビルドする。

#### 補足：静的リンク

1. vcpkgで静的ライブラリをインストールする：
    ```
    PS> .\vcpkg install sdl2:x64-windows-static sdl2-image:x64-windows-static sdl2-mixer:x64-windows-static sdl2-ttf:x64-windows-static lua:x64-windows-static sol2:x64-windows-static
    ```
1. [マニュアル](https://github.com/Microsoft/vcpkg/blob/master/docs/users/integration.md#triplet-selection)を参考にshooter2d.vcxprojを書き替える。具体的には、shooter2d.vcxprojを適当なテキストエディタで開いて、PropatyGroupタグの中身にVcpkgTripletを追加する：
    ```
    <PropertyGroup Label="Globals">
      <!-- .... -->
      <VcpkgTriplet Condition="'$(Platform)'=='Win32'">x86-windows-static</VcpkgTriplet>
      <VcpkgTriplet Condition="'$(Platform)'=='x64'">x64-windows-static</VcpkgTriplet>
    </PropertyGroup>
    ```
1. shooter2d.slnをVisual Studioで開いて「プロジェクト」->「プロパティ」->「リンカー」->「入力」->「追加の依存ファイル」に opengl32.lib, version.lib, winmm.lib, imm32.lib, setupapi.lib を書き加える。
1. Visual StudioからReleaseビルドする。

### Linux OS

#### Meson + Ninja（推奨）

1. ソースをダウンロードする。注意：ダウンロードされたファイルの改行コードがLF (Linux) になっていないと、`meson build` でエラーが発生することがある。
1. パッケージマネージャーなどを用いてSDLとSDLのライブラリとLuaをインストールする。例えば、Debian系ならAPTからインストールできる：
    ```bash
    $ apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev liblua5.3-dev
    ````
1. パッケージマネージャーあるいはpip3を利用して、MesonとNinjaをインストールする。pip3を使う場合は `python3 -m pip install meson ninja` などとコマンドを叩く。
1. 次のコマンドを実行してビルドする：
    ```bash
    $ cd shooter2d
    $ meson build
    $ cd build
    $ ninja
    ```
    このとき、sol2が自動的にダウンロードされる。もし `meson build` の時にLuaが無いと怒られる場合は、shooter2d/subprojects/sol2-\*.\*.\*/subprojects/lua.wrap を shooter2d/subprojects ディレクトリにコピーする。ビルドが成功すれば、shooter2d/build/shooter2dディレクトリに実行可能ファイルが生成される。
1. 素材をダウンロードして、shooter2d/build/shooter2d/images/\*.png および shooter2d/build/shooter2d/images/\*.wav に配置する（下記の図を参考にされたい）。
1. 実行時にフォントが見つからないというエラーが出る場合は、user_interface.cpp を修正する。

#### GNU Make

1. ソースをダウンロードする。
1. パッケージマネージャーなどを用いてSDLとSDLのライブラリとLuaをインストールする。例えば、Debian系ならAPTからインストールできる：
    ```bash
    $ apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev liblua5.3-dev
    ````
1. sol2は別途ダウンロードして、shooter2d/packages/sol2/sol.hpp に配置する。
1. shooter2d/shooter2d/images ディレクトリと shooter2d/shooter2d/se ディレクトリを作成する。素材をダウンロードして、images と se それぞれにpngファイルとwavファイルを配置する（下記の図を参考にされたい）。
1. shooter2d/shooter2d/lib ディレクトリを作成する。
1. shooter2d/shooter2d に移動して、`make debug` または `make release` を実行する。
1. 実行時にフォントが見つからないというエラーが出る場合は、user_interface.cpp を修正する。

### ディレクトリ構成 (2019/07/15)

~~~
shooter2d
├── meson.build
├── packages
│   └── sol2
│       └── sol.hpp
├── shooter2d
│   ├── Makefile
│   ├── grabber.sh
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
│   ├── meson.build
│   ├── scripts
│   │   ├── grabber.sh
│   │   ├── meson.build
│   │   └── *.lua
│   ├── shooter2d.vcxproj
│   ├── se
│   │   ├── enemy_damage.wav
│   │   ├── enemy_vanish_effect-A.wav
│   │   ├── nc899.wav
│   │   ├── sha04.wav
│   │   └── shot1.wav
│   ├── *.cpp
│   └── *.h
├── shooter2d.sln
└── subprojects
     ├── sol2.wrap
     └── (lua.wrap)
~~~

## ライセンス

This software is distributed under the terms of the zlib license reproduced [here](LICENSE).
