# SHFXRegularPolygon.aip

__SHFXRegularPolygon.aip__ は、Adobe Illustrator  2020 (mac/win) プラグインです。

選択したパスを正多角形に変換する効果（エフェクト）を追加します。

機能としては単純ですが、[Dear ImGui](https://github.com/ocornut/imgui) で LiveEffectプラグインの設定ダイアログを作成する例として公開しています。

## 使い方

![image](https://gist.github.com/shspage/cfa3496f862b21c27b7a1157690d335a/raw/59e143430b4b1db5a78fe51b478e8c9a000c1836/effect.jpg)  
動画 - 
https://twitter.com/shspage/status/1256437710778036224

（動画中のダイアログは初期バージョンのものです。）

## 設定ウィンドウの項目：

* scale(%) : 拡大/縮小率を百分率で設定します。0.1未満は設定できません。
* angle : 回転角度を設定します。
* vertices : 頂点の数を設定します。3未満は設定できません。


## 動作環境：

Adobe Illustrator 2020 (mac/win)

Windows10 (win版)


## 開発環境

Adobe Illustrator 2020 SDK  
Xcode 10.1 / macOS 10.13 sdk / MacOS High Sierra

Visual Studio 2017 / Windows10


## ビルド

### Mac

myImGuiDialog_mac/myImGuiDialog.xcodeproj で libmyImGuiDialog.a をビルドしてから、
SHFXRegularPolygon.xcodeproj で SHFXRegularPolygon.aip をビルドしてください。


### Windows

SHFXRegularPolygon.sln でソリューションのビルドを実行してください。myImGuiDailog_win、SHFXRegularPolygon の順でプロジェクトがビルドされます。

## 補足（ビルド）
<!-- ソースコードは https://github.com/shspage/SHFXRegularPolygon_aip にあります。ビルドする際は以下をご一読ください。-->
* SHFXRegularPolygon_aip フォルダはIllustrator SDKのsamplecodeフォルダの直下に置いてください。
* (Windows) 添付のプロジェクトファイルでビルドするには、ソースコード(.cpp, .h, .hpp)の文字コードをUTF-8からMultibyte(cp932)に変換する必要があります。

## インストール

（ビルド後）
__SHFXRegularPolygon.aip__ をAdobe Illustrator のプラグインフォルダ、または追加プラグインフォルダに 置いてください。

次回 Illustrator の実行時にプラグインが読み込まれ、「効果」メニューに「StayHomeFX」が追加されます。

オブジェクトを選択したうえで、サブメニューの「Regular Polygon -SHFX」を選択すると効果の設定ウィンドウが表示されます。


## ライセンス

MIT License  
Copyright (c) 2020 Hiroyuki Sato  
https://github.com/shspage  
詳細は LICENSE.txt をご覧ください。


以下については、それぞれのライセンスを参照ください。

* Dear ImGui : Copyright (c) 2014-2020 Omar Cornut  
Licensed under the MIT License  
https://github.com/ocornut/imgui

* Adobe Illustrator 2020 SDK  
Copyright (c) 2020 Adobe. All rights reserved.  
https://www.adobe.io/



