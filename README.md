# SHFXRegularPolygon.aip

[readme in japanese](https://github.com/shspage/SHFXRegularPolygon_aip/blob/master/readme_ja.md)

__SHFXRegularPolygon.aip__  is a plug-in for Adobe Illustrator 2020-2021 (mac/win).

Adds an effect that converts the selected path to a regular polygon.

The function is rather simple, actually this repo is an example of creating a LiveEffect plugin configuration dialog with [Dear ImGui](https://github.com/ocornut/imgui) .

## How To Use

![image](https://gist.github.com/shspage/cfa3496f862b21c27b7a1157690d335a/raw/59e143430b4b1db5a78fe51b478e8c9a000c1836/effect.jpg)  
video - 
https://twitter.com/shspage/status/1256437710778036224

(The dialog in the video is from the initial version.)

## Items in the Settings Window

* scale(%) :  Sets the zoom in/out ratio in percentages. (lower limit is 0.1)
* angle : Sets the rotation angle.
* vertices : Sets the number of vertices. (lower limit is 3)


## Working Environment

Adobe Illustrator 2020-2021 (mac/win)

Windows10 (win version)


## Development Environment

Adobe Illustrator 2021 SDK  
Xcode 12.2 / macOS 11.0 sdk / MacOS Big Sur

<!-- Visual Studio 2017 / Windows10 -->


## Build

### Mac

Open _myImGuiDialog_mac/myImGuiDialog.xcodeproj_ and build _libmyImGuiDialog.a_ first.
Then open _SHFXRegularPolygon.xcodeproj_ and build _SHFXRegularPolygon.aip_

### Windows

Open _SHFXRegularPolygon.sln_ and build solution.  
Projects will be built in the order of _myImGuiDailog_win_ and _SHFXRegularPolygon_ .


## Note For Build

* Place "SHFXRegularPolygon_aip" folder directly under Illustrator SDK's samplecode folder.
* (Windows) To build with the attached project file, it is necessary to convert the character code of the source code (.cpp, .h, .hpp) from UTF-8 to Multibyte(cp932).


## Installation

(After build)
Place __SHFXRegularPolygon.aip__ in the Adobe Illustrator plugin folder or additional plugin folder.

The next time Illustrator runs, the plugin will be loaded and "StayHomeFX" will be added to the "Effects" menu.

After selecting a path, select "Regular Polygon -SHFX" from the submenu to show the effect setting window.

Note: "StayHomeFX" is a temporary name, and will be changed to another one at a later date. The abbreviation SHFX remains the same.

## License

MIT License  
Copyright (c) 2020 Hiroyuki Sato  
https://github.com/shspage  
See LICENSE.txt for details.

This software depends on the following resources which may have the licenses differs from this software.

* Dear ImGui : Copyright (c) 2014-2020 Omar Cornut  
Licensed under the MIT License  
https://github.com/ocornut/imgui

* Adobe Illustrator 2020 SDK  
Copyright (c) 2020 Adobe. All rights reserved.  
https://www.adobe.io/

